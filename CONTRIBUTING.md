# Contributing

This repository packages focused DeepJIT fixes and verification evidence.

- Keep each patch scoped to one upstream issue.
- Base patches on the commit recorded in the README.
- Add a failing regression before the fix whenever practical.
- Run the Ubuntu host smoke workflow.
- Do not claim CUDA/Ascend validation without the corresponding toolchain and device.
- Do not include credentials, private cache paths, or proprietary kernel source.

Upstream fixes should normally be offered as small PRs to `deepseek-ai/DeepJIT` rather than as competing implementations.
