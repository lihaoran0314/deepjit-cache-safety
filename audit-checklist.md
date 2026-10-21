# DeepJIT patch audit checklist

## Patch integrity

- [ ] Patch applies cleanly to `18fe520b0473fc45d32d2f0e86923da707de19ab`
- [ ] Patch reverses cleanly from the prepared branch
- [ ] `git diff --check` passes
- [ ] No unrelated formatting or generated-file changes

## Host smoke

- [ ] Ubuntu 24.04 with build-essential and libdw-dev
- [ ] `bash scripts/validate-on-ubuntu.sh`
- [ ] Directory symlink target survives cleanup
- [ ] Dangling symlink is removed
- [ ] Failed publication throws instead of setting hit/committed
- [ ] Temporary payload remains after failed publication
- [ ] Incomplete destination is not modified
- [ ] Later committed competitor entry is safely reused

## Upstream suites

- [ ] CUDA test project on supported Linux/Torch/CUDA environment
- [ ] Ascend test project on supported CANN environment
- [ ] Existing concurrent cache publication tests still pass
- [ ] Existing crashed writer recovery tests still pass
- [ ] No shared-cache SGID/permission regression

## CUDA context contract

- [ ] Upstream maintainers agree that one Runtime is context-affine
- [ ] README wording matches actual CUDA Driver API behavior
- [ ] If runtime checks are required, add a separate patch rather than mixing concerns
