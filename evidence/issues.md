# 上游证据

## DeepJIT Issue #2

<https://github.com/deepseek-ai/DeepJIT/issues/2>

核心复现：`safe_remove_all` 使用 path-based `exists` / `is_directory`，会跟随目录符号链接后递归删除目标内容。

## DeepJIT Issue #3

<https://github.com/deepseek-ai/DeepJIT/issues/3>

核心复现：目标为非空但未提交的目录时，rename 失败；当前实现删除完成后的临时 payload，并把失败路径标记为 hit/committed。

## DeepJIT Issue #30

<https://github.com/deepseek-ai/DeepJIT/issues/30>

核心问题：内存 cache key 不包含 CUDA context，`CUfunction` 缓存在 context 内有效，跨 device/context 复用的契约不明确。

本次对 #30 采用文档契约处理，不改变缓存 key；若上游认为需要运行时检测，可另开补丁。
