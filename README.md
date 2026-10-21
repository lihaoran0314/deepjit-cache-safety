# DeepJIT 缓存安全修复包

基线提交：`18fe520b0473fc45d32d2f0e86923da707de19ab`  
修复分支提交：`148d2ed9044c75d3704028a06e4b759cf8e12de5`  
状态：**补丁已生成，等待审计和 CI 编译验证**

## 对应问题

- [Issue #2](https://github.com/deepseek-ai/DeepJIT/issues/2)：临时缓存清理跟随目录符号链接，可能删除链接目标内容。
- [Issue #3](https://github.com/deepseek-ai/DeepJIT/issues/3)：磁盘缓存发布失败仍返回成功，且可能删除已完成临时 payload。
- [Issue #30](https://github.com/deepseek-ai/DeepJIT/issues/30)：CUDA Runtime/Kernel 缓存的 context/device 归属不明确。

## 修复内容

### 1. 符号链接安全清理

`safe_remove_all` 现在使用 `symlink_status`：

- 不跟随目录符号链接；
- 删除符号链接本身；
- 支持 dangling symlink；
- 普通文件、目录和空目录行为保持不变。

### 2. 缓存发布成功判定

`DiskCacheEntry::commit()` 不再把所有 rename 失败都当成“竞争写者已成功发布”。

现在只有目标目录存在 `.committed` 标记时，才复用竞争写者结果；否则：

- 抛出包含原 rename 错误的异常；
- 保留临时目录中的完整 payload，便于重试；
- 不修改不完整目标目录；
- 对象析构时仍执行 best-effort 清理。

### 3. CUDA context 契约

README 和 `Kernel` 注释明确：

- `Runtime<CUDA>` 和 Kernel 句柄属于创建时的 CUDA context/device；
- 不跨 context 或异构设备共享 Runtime；
- 同一架构的 CUBIN 可以通过独立 Runtime 复用磁盘缓存。

## 测试改动

CUDA 和 Ascend 测试项目均增加：

- 目录符号链接不删除目标；
- dangling symlink 可被清理；
- rename 失败且目标未提交时必须报错；
- 失败后临时 payload 保留；
- 失败后不修改不完整目标；
- 竞争写者随后提交 `.committed` 时允许安全复用。

## 文件

- `patches/0001-Fix-disk-cache-publication-and-symlink-safe-cleanup.patch`
- `tests/cache_safety_smoke.cpp`
- `scripts/validate-on-ubuntu.sh`
- `.github/workflows/validate-patch.yml`
- `evidence/issues.md`

## 当前验证状态

本机为 Windows，缺少 DeepJIT 所需的 Linux/C++20、elfutils、CUDA/NVCC 或 Ascend 工具链，因此没有在本地编译。

GitHub Actions 的 Ubuntu 24.04 host smoke 已成功执行：

- https://github.com/lihaoran0314/deepjit-cache-safety/actions/runs/39524297315

该验证覆盖目录 symlink、dangling symlink、发布失败保留临时 payload，以及竞争写者提交后的安全复用。CUDA/Ascend 全量上游测试仍需对应设备工具链。

## 许可证

DeepJIT 原始代码仍适用其 MIT 许可证。本修复包只包含补丁、测试和说明。
