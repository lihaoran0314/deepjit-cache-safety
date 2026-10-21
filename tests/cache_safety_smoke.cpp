// SPDX-License-Identifier: MIT
// Host-only smoke test for DeepJIT cache publication and symlink cleanup.

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>

#include <deep_jit/cache/disk.hpp>
#include <deep_jit/utils/filesystem.hpp>

namespace fs = std::filesystem;

static void require(const bool condition, const std::string& message) {
    if (not condition)
        throw std::runtime_error(message);
}

int main() {
    const auto root = fs::path("/tmp") / ("deepjit-cache-safety-" + std::to_string(::getpid()));
    deep_jit::safe_remove_all(root);
    deep_jit::make_dirs(root);

    try {
        const auto target = root / "symlink_target";
        deep_jit::make_dirs(target / "nested");
        deep_jit::write_file_sync(target / "nested/payload", "keep");
        const auto link = root / "directory_link";
        fs::create_directory_symlink(target, link);
        deep_jit::safe_remove_all(link);
        require(not fs::exists(link), "directory symlink was not removed");
        require(fs::is_regular_file(target / "nested/payload"), "symlink target was deleted");
        deep_jit::safe_remove_all(target);

        const auto dangling_link = root / "dangling_link";
        fs::create_symlink(root / "missing", dangling_link);
        deep_jit::safe_remove_all(dangling_link);
        require(fs::symlink_status(dangling_link).type() == fs::file_type::not_found, "dangling symlink remained");

        deep_jit::DiskCache cache({root / "primary", root / "secondary"});
        const auto destination = root / "primary/cache/publication.digest";
        deep_jit::make_dirs(destination);
        deep_jit::write_file_sync(destination / "partial", "partial");

        auto entry = cache.entry("publication", "digest");
        deep_jit::write_file_sync(entry.path / "payload", "complete");
        const auto temporary_path = entry.path;
        bool failed = false;
        try {
            (void)entry.commit();
        } catch (const std::exception&) {
            failed = true;
        }
        require(failed, "commit reported success for an uncommitted destination");
        require(not entry.hit, "failed commit marked the entry as a hit");
        require(fs::is_regular_file(temporary_path / "payload"), "failed commit deleted the temporary payload");
        require(fs::is_regular_file(destination / "partial"), "failed commit modified the incomplete destination");

        deep_jit::write_file_sync(destination / deep_jit::kCommitFileName, "");
        const auto winner = entry.commit();
        require(winner == destination, "retry did not reuse the concurrently published entry");
        require(cache.entry("publication", "digest").hit, "published entry was not visible");
        require(not fs::exists(temporary_path), "temporary payload remained after reuse");
    } catch (...) {
        deep_jit::safe_remove_all(root);
        throw;
    }

    deep_jit::safe_remove_all(root);
    std::cout << "cache safety smoke test passed\n";
    return 0;
}
