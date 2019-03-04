from typing import Any, IO, Iterator, Tuple

class JsonSlicer:
    def __init__(self,
                 file: IO,
                 path_prefix: Tuple[Any[str, bytes, None], ...],
                 read_size: int=...,
                 path_mode: str=...,
                 yajl_allow_comments: bool=...,
                 yajl_dont_validate_strings: bool=...,
                 yajl_allow_trailing_garbage: bool=...,
                 yajl_allow_multiple_values: bool=...,
                 yajl_allow_partial_values: bool=...,
                 encoding: Any[None, str]=...,
                 errors: Any[None, str]=...,
                 binary: bool=...) -> None: ...

    def __iter__(self) -> Iterator: ...

    def __next__(self) -> Any: ...
