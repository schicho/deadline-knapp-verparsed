# Joern Solving Example


**NOTE**: Im Moment findet joern nicht direkt den path zum system() call.
Irgendwo in der mitte (bei `lex_token` und `buffer_append` in `config_serialize.c) verliert es die Spur.

Das macht es alles natürlich schwieriger. ich bin noch auf der suche nach einer lösung...

1. Start joern in the configloader directory.
2. import the code by typing into the joern shell:

```sc
importCode(inputPath=".", projectName="ctf")
```

3. define sink and source

```sc
joern> def sink = cpg.call("sys_unsafe_log_info").argument
```

```sc
def source = cpg.call("open_input_files")
```


4. get flows

```sc
joern> sink.reachableByFlows(source).p
```

```
val res113: List[String] = List(
  """
┌──────────────────┬─────────────────────────────────────────────────────┬────┬──────────────────────────┬────────────┐
│nodeType          │tracked                                              │line│method                    │file        │
├──────────────────┼─────────────────────────────────────────────────────┼────┼──────────────────────────┼────────────┤
│Call              │open_input_files(n, cli->args)                       │178 │run                       │main.c      │
│Identifier        │** in_files = open_input_files(n, cli->args)         │178 │run                       │main.c      │
│Identifier        │!in_files                                            │180 │run                       │main.c      │
│Identifier        │merge_config_files(out_file, n, in_files)            │187 │run                       │main.c      │
│MethodParameterIn │merge_config_files(FILE* out, int n, FILE** in)      │94  │merge_config_files        │main.c      │
│MethodParameterOut│RET                                                  │94  │merge_config_files        │main.c      │
│Identifier        │merge_config_files(out_file, n, in_files)            │187 │run                       │main.c      │
│Identifier        │close_input_files_and_free(n, in_files)              │191 │run                       │main.c      │
│MethodParameterIn │close_input_files_and_free(const int n, FILE** files)│49  │close_input_files_and_free│main.c      │
│Identifier        │!files                                               │50  │close_input_files_and_free│main.c      │
│Call              │* f = files[i]                                       │54  │close_input_files_and_free│main.c      │
│Identifier        │* f = files[i]                                       │54  │close_input_files_and_free│main.c      │
│Identifier        │!f                                                   │55  │close_input_files_and_free│main.c      │
│Identifier        │filesystem_close(f)                                  │56  │close_input_files_and_free│main.c      │
│MethodParameterIn │filesystem_close(FILE* file)                         │79  │filesystem_close          │filesystem.c│
│Identifier        │file_sanity_check(file)                              │80  │filesystem_close          │filesystem.c│
│MethodParameterIn │file_sanity_check(const FILE* file)                  │46  │file_sanity_check         │filesystem.c│
│Identifier        │file == NULL                                         │47  │file_sanity_check         │filesystem.c│
│MethodParameterOut│RET                                                  │46  │file_sanity_check         │filesystem.c│
│Identifier        │file_sanity_check(file)                              │80  │filesystem_close          │filesystem.c│
│Identifier        │fs_low_close(file)                                   │85  │filesystem_close          │filesystem.c│
│MethodParameterIn │fs_low_close(FILE* file)                             │73  │fs_low_close              │fs_low.c    │
│MethodParameterOut│RET                                                  │73  │fs_low_close              │fs_low.c    │
│Identifier        │fs_low_close(file)                                   │85  │filesystem_close          │filesystem.c│
│Call              │(void*)file                                          │91  │filesystem_close          │filesystem.c│
└──────────────────┴─────────────────────────────────────────────────────┴────┴──────────────────────────┴────────────┘""",
  """
┌──────────────────┬────────────────────────────────────────────────────────────────────────────────────────────────────────────────┬────┬────────────────────┬────────────────────────┐
│nodeType          │tracked                                                                                                         │line│method              │file                    │
├──────────────────┼────────────────────────────────────────────────────────────────────────────────────────────────────────────────┼────┼────────────────────┼────────────────────────┤
│Call              │open_input_files(n, cli->args)                                                                                  │178 │run                 │main.c                  │
│Identifier        │** in_files = open_input_files(n, cli->args)                                                                    │178 │run                 │main.c                  │
│Identifier        │!in_files                                                                                                       │180 │run                 │main.c                  │
│Identifier        │merge_config_files(out_file, n, in_files)                                                                       │187 │run                 │main.c                  │
│MethodParameterIn │merge_config_files(FILE* out, int n, FILE** in)                                                                 │94  │merge_config_files  │main.c                  │
│Identifier        │!in                                                                                                             │96  │merge_config_files  │main.c                  │
│Call              │config_deserialize(in[i])                                                                                       │102 │merge_config_files  │main.c                  │
│MethodParameterIn │config_deserialize(FILE* stream)                                                                                │353 │config_deserialize  │configfile_deserialize.c│
│Identifier        │!stream                                                                                                         │354 │config_deserialize  │configfile_deserialize.c│
│Identifier        │parse_contents(main_block, stream, 0)                                                                           │363 │config_deserialize  │configfile_deserialize.c│
│MethodParameterIn │parse_contents(config_block* block, FILE* stream, int expect_closing_brace)                                     │322 │parse_contents      │configfile_deserialize.c│
│Identifier        │lex_token(stream)                                                                                               │324 │parse_contents      │configfile_deserialize.c│
│MethodParameterIn │lex_token(FILE* stream)                                                                                         │105 │lex_token           │configfile_deserialize.c│
│Identifier        │skip_ws_and_comments(stream)                                                                                    │108 │lex_token           │configfile_deserialize.c│
│MethodParameterIn │skip_ws_and_comments(FILE* stream)                                                                              │80  │skip_ws_and_comments│configfile_deserialize.c│
│Identifier        │fgetc(stream)                                                                                                   │82  │skip_ws_and_comments│configfile_deserialize.c│
│MethodParameterOut│RET                                                                                                             │80  │skip_ws_and_comments│configfile_deserialize.c│
│Identifier        │skip_ws_and_comments(stream)                                                                                    │108 │lex_token           │configfile_deserialize.c│
│Identifier        │fgetc(stream)                                                                                                   │113 │lex_token           │configfile_deserialize.c│
│Call              │fgetc(stream)                                                                                                   │113 │lex_token           │configfile_deserialize.c│
│Identifier        │ch = fgetc(stream)                                                                                              │113 │lex_token           │configfile_deserialize.c│
│Identifier        │ch == EOF                                                                                                       │114 │lex_token           │configfile_deserialize.c│
│Identifier        │ch == EOF                                                                                                       │114 │lex_token           │configfile_deserialize.c│
│Identifier        │ch == EOF                                                                                                       │140 │lex_token           │configfile_deserialize.c│
│Identifier        │ch == EOF                                                                                                       │156 │lex_token           │configfile_deserialize.c│
│Identifier        │ch == EOF                                                                                                       │156 │lex_token           │configfile_deserialize.c│
│Identifier        │sys_unsafe_log_info(
                        "unterminated escape sequence encountered during deserialization:  │158 │lex_token           │configfile_deserialize.c│
│                  │%c", ch)                                                                                                        │    │                    │                        │
└──────────────────┴────────────────────────────────────────────────────────────────────────────────────────────────────────────────┴────┴────────────────────┴────────────────────────┘"""
)
```

