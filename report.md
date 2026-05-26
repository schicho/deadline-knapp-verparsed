# Project Title: `Deadline knapp verPARSEd`

Group Members: `Johann Sebastion Schicho, 12405408; Sophie Pokorny 01633652; Tim Manuel Lehner, 12434704`

## Introduction & Problem Field

```
For CTF:
- Describe the core idea and vulnerability.
- Why is this specific vulnerability relevant?
```

We implemented a web service, from which you can download a C program that implements a configuration loader.
The configuration loader parses and validates nginx configuration files.
The configuration loader is a program that may read multiple config files and parses them and outputs one combined configuration.

Somewhere hidden layers deep in this configuration loader, through a specially crafted config file a `system()` call may be executed with arbitrary user input.

**The flag is the name of a function, which given a malicious configuration file, starts the chain to execute arbitrary shell commands.**

The challenge is that the C code is obfuscated randomly per download and the flag needs to be entered within 60 seconds in the web service. Thus, manual search through the code is not possible.

Our vulnerability is an (ordered on Temu) hommage to the Log4Shell vulnerability, which insecurely trusts user input without proper sanitization in a logging call, which allows arbitrary code execution.

Our vulnerability is relevant as improper input sanitization is a story that keeps on giving.
This problem is getting more severe with AI Agents producing code which looks fine but contains attack vectors which are not easily deteced on first sight.

By solving this challenge users get familiar with the tool Joern,
which allows code analyzation using code property graphs. They learn how to use the tool and how to search and detect flows in programs.

## Core Work

```
For CTF:
- Detail the architecture of your challenge and design decisions you made.
- What is the intended solution path (i.e. from not-knowing to getting the flag)?
- What is the necessary knowledge of someone trying to solve the challenge?

For Both:
- What technical problems did you encounter, and how did you handle them?
```
### Architecture
Our code is split into three main components which also resemble the main contents of our challenge:
Firstly the configloader contains the actual unobfuscated code of the nginx configloader. This code can be run and contains a vulnerability which enables to run arbitrary system calls.

Secondly, the obfuscator obfuscates the configloader code, i.e. file, function and variables names, and macros and puts them into a seperate subfolder. We paid attention that the output produced by the obfuscator can still be compiled so the students could actually run the program by themselves and play around with it.
The obfuscator does rather simple obfuscation since we do not want to make the obfuscation and thus the challenge too hard. Further, most open source obfuscators are working on a compiler level than on a C level, this however would complicate the challenge even more.

Lastly, the webservice contains the code for actually running the challenge. It contains a Dockerfile which can be run for easy startup. The students can then access the challenge website, where they can press a button to start the challenge. This runs the obfuscator and lets the user download the freshly obfuscated files.
The user has then a time window of 60 seconds to analyze the code and return the name of the correct function name to solve the challenge successfully.

![Picture of our proposed architecture](architecture.svg)


### Solution Path

The intended solve path is to understand the program generally and find the vulnerability, even though the timer of 60 seconds has then long run out. Next, the attacker should write a `joern` script, which can find the attack path, i.e. the vulnerable function's name, regardless of obfuscation.

Executing the `joern` script on top of a newly downloaded and obfuscated version of the program allows the attacker to find the vulnerable function's name within 60 seconds and solve the challenge by entering the name into the web service.

Finding the right function name can be achieved by running following commands:

1. Load the configloaders code using Joern

```
joern> importCode(inputPath=".", projectName="ctf")
Using generator for language: NEWC: CCpgGenerator
Creating project `ctf` for code at `.`
[...]
val res1: io.shiftleft.codepropertygraph.generated.Cpg = Cpg[Graph[6708 nodes]]
```

2. Find the methods that end up flowing to `system()`

```                                                                             
joern> cpg.call("system").method.name.l
val res2: List[String] = List("sys_unsafe_run", "sys_unsafe_sanitize_run", "sys_unsafe_whitelist_run")
                                                                                
joern> cpg.call("sys_unsafe_run").method.name.l
val res3: List[String] = List("sys_unsafe_vlog")
                                                                                
joern> cpg.call("sys_unsafe_vlog").method.name.l
val res4: List[String] = List(
  "sys_unsafe_log_lvl",
  "sys_unsafe_log_info",
  "sys_unsafe_log_debug",
  "sys_unsafe_log_warning",
  "sys_unsafe_log_error"
)
```

3. Find out that `"sys_unsafe_log_info"` directly prints a block name without sanitization. There are five different calls to that log function, but only one takes a user input string.

```
joern> cpg.call("sys_unsafe_log_info")
val res10: Iterator[io.shiftleft.codepropertygraph.generated.nodes.Call] = non-empty iterator
                                                                                                                                                                                        
joern> cpg.call("sys_unsafe_log_info").l
val res11: List[io.shiftleft.codepropertygraph.generated.nodes.Call] = List(
  Call(
    argumentIndex = -1,
    argumentLabel = None,
    argumentName = None,
    code = """sys_unsafe_log_info(
                    "unterminated quoted token encountered during deserialization: EOF")""",
    columnNumber = Some(value = 17),
    dispatchType = "STATIC_DISPATCH",
    dynamicTypeHintFullName = IndexedSeq(),
    lineNumber = Some(value = 141),
    methodFullName = "sys_unsafe_log_info",
    name = "sys_unsafe_log_info",
    offset = None,
    offsetEnd = None,
    order = 1,
    possibleTypes = IndexedSeq(),
    signature = "",
    staticReceiver = None,
    typeFullName = "void"
  ),
  Call(
    argumentIndex = -1,
    argumentLabel = None,
    argumentName = None,
    code = """sys_unsafe_log_info(
                        "unterminated escape sequence encountered during deserialization: %c", ch)""",
    columnNumber = Some(value = 21),
    dispatchType = "STATIC_DISPATCH",
    dynamicTypeHintFullName = IndexedSeq(),
    lineNumber = Some(value = 157),
    methodFullName = "sys_unsafe_log_info",
    name = "sys_unsafe_log_info",
    offset = None,
    offsetEnd = None,
    order = 1,
    possibleTypes = IndexedSeq(),
    signature = "",
    staticReceiver = None,
    typeFullName = "void"
  ),
  Call(
    argumentIndex = -1,
    argumentLabel = None,
    argumentName = None,
    code = "sys_unsafe_log_info(\"configuration deserialization completed successfully\")",
    columnNumber = Some(value = 5),
    dispatchType = "STATIC_DISPATCH",
    dynamicTypeHintFullName = IndexedSeq(),
    lineNumber = Some(value = 371),
    methodFullName = "sys_unsafe_log_info",
    name = "sys_unsafe_log_info",
    offset = None,
    offsetEnd = None,
    order = 6,
    possibleTypes = IndexedSeq(),
    signature = "",
    staticReceiver = None,
    typeFullName = "void"
  ),
  Call(
    argumentIndex = -1,
    argumentLabel = None,
    argumentName = None,
    code = "sys_unsafe_log_info(\"%s\", block_name)",
    columnNumber = Some(value = 5),
    dispatchType = "STATIC_DISPATCH",
    dynamicTypeHintFullName = IndexedSeq(),
    lineNumber = Some(value = 43),
    methodFullName = "sys_unsafe_log_info",
    name = "sys_unsafe_log_info",
    offset = None,
    offsetEnd = None,
    order = 3,
    possibleTypes = IndexedSeq(),
    signature = "",
    staticReceiver = None,
    typeFullName = "void"
  ),
  Call(
    argumentIndex = -1,
    argumentLabel = None,
    argumentName = None,
    code = "sys_unsafe_log_info(\"filesystem_close: closed handle=%p\", (void*)file)",
    columnNumber = Some(value = 5),
    dispatchType = "STATIC_DISPATCH",
    dynamicTypeHintFullName = IndexedSeq(),
    lineNumber = Some(value = 91),
    methodFullName = "sys_unsafe_log_info",
    name = "sys_unsafe_log_info",
    offset = None,
    offsetEnd = None,
    order = 5,
    possibleTypes = IndexedSeq(),
    signature = "",
    staticReceiver = None,
    typeFullName = "void"
  )
)
```

4. Enter the obfusctaed calling functions name representing `"serialize_block"` as flag on the website. Note that of course the files and function names are obfuscated, so the flag is a random string and not `"serialize_block"`.

```
joern> cpg.call("sys_unsafe_log_info").method.l
val res13: List[io.shiftleft.codepropertygraph.generated.nodes.Method] = List(
  [...]
  Method(
    astParentFullName = "configfile_serialize.c:<global>",
    astParentType = "TYPE_DECL",
    code = [...],
    columnNumber = Some(value = 1),
    columnNumberEnd = Some(value = 1),
    filename = "configfile_serialize.c",
    fullName = "serialize_block",                     <------------------
    genericSignature = "<empty>",
    hash = None,
    isExternal = false,
    lineNumber = Some(value = 40),
    lineNumberEnd = Some(value = 79),
    name = "serialize_block",
    offset = None,
    offsetEnd = None,
    order = 1,
    signature = "int(config_block*,int,FILE*)"
  ),
  [...]
)
```


To solve the challenge quickly a complete query for Joern can be created:

```
joern> cpg.call("system")
        .method
        .caller 
        .caller
        .callIn
        .filter(_.code.contains("%s"))
        .method
        .name
        .l
val res7: List[String] = List("serialize_block", "cfgf_easy_add_directive")
```

#### Additional information

The vulnerability can be triggered when running the program in verbose mode only, given a specially crafted config file as in the following.

A block needs to be named a quoted shell command, which ends the intended `system()` call with a semicolon:

```
http {
    ";ls" {   
    }
}
```

Example output:

```
./configloader -v test/insecure.cfg 
Di 26. Mai 14:48:43 CEST 2026
Linux heast 6.17.0-29-generic #29~24.04.1-Ubuntu SMP PREEMPT_DYNAMIC Mon May 11 10:30:58 UTC 2 x86_64 x86_64 x86_64 GNU/Linux
[INFO] Output File: -
[INFO] Input 1: test/insecure.cfg
[DEBUG] filesystem_open: path='test/insecure.cfg', mode='r'
[INFO] opened path='test/insecure.cfg' with mode='r'
[DEBUG] parsing statement starting with 'http'
[DEBUG] parsing statement starting with ';ls'
[INFO] configuration deserialization completed successfully
[DEBUG] serializing configuration
[INFO] main
[INFO] http
http {
[INFO]
build                     configfile_deserialize.h  configfile_merge.h      filesystem.c  joern_example.md  printer.c  system.h
cli.c                     configfile_easy.c         configfile_serialize.c  filesystem.h  logger.c          printer.h  sysunsafe.c
cli.h                     configfile_easy.h         configfile_serialize.h  fs_low.c      logger.h          readme.md  sysunsafe.h
configfile.c              configfile.h              configloader            fs_low.h      main.c            sysinfo.c  test
configfile_deserialize.c  configfile_merge.c        expl01t.txt             fsops.h       Makefile          sysinfo.h  workspace
    ;ls {
    }
}
[INFO] filesystem_close: closed handle=0x7f8b8bc045c0
[INFO] filesystem_close: closed handle=0x5cc746d145e0
```


### Starting the webservice
The webservice is running in a Docker container and can be started with docker compose. This command is the only one which is needed to run this challenge, all others are completely optional and for further exploration.
In project root run::
```bash
docker compose up
```

You can then reach the website under http://127.0.0.1:5000 or http://172.18.0.2:5000.
If these links do not work, check out the docker output for the correct link.

This runs the server in dev mode; to run it in production mode (using `waitress`), run
```bash
SERVER_MODE=production docker compose up --build 
```

#### Debugging
If you want to debug the webservice locally, you can do so by simply running the `app.py`.

First install the venv:
```bash
# from project root
cd webservice/
python3 -m venv ./venv
source ./venv/bin/activate
pip install -r ./requirements.txt
```

Then run the webservice:
```bash
python3 webservice.py
```

By default, the webservice clears the obfuscated files from memory after the initial request finished and only keeps the flag to the obfuscated code. If you want to keep the obfuscated code for further obfuscation, run
```bash
export FLASK_DEBUG=1
python3 webapp.py
```  

Note that the obfuscated code can also be inspected by running the obfuscator on their own (see next chapter).

### Runnig the configloader
Running the configloader code is not necessary to solve the challenge.
However if you want to do so, you can by first compiling the application with the included MAKE file:
```bash
cd ./configloader
make
```

If you want to run and inspect the obfuscated code, you can do so by first running the obfuscator.
First install the Python venv:
```bash
# from project root
cd obfuscator/
python3 -m venv ./venv
source ./venv/bin/activate
pip install -r ./requirements.txt
```

Then run the obfuscator:
```bash
python3 obfuscator.py
```

You can then inspect the output in `obfuscator/out`.
If you want to run this, you can do that again similiarly to the original code:
```bash
cd out/
make
```



### Knowledge needed for this challenge
To solve this challenge, the user needs to know how system calls work and what makes them unsafe. They also need to know how to work with joern, this can however be learned rather quickly or the user could be supported by giving hints if they are stuck.

### Technical Problems and other hurdles

#### Joern Flows

It would have been really nice to trace a flow in Joern from opening a file to calling `system()` with it's input.
Unfortunately, Joern looses track of a flow if some code is "too complicated". In our case it happened in the parsing code.
The flow was tracked from file loading, opening a stream, passing it thorugh various functions, until a `fgetc()`. However, as soon
as the char was written into a buffer, the flow ended.

Hence, our initial idea of defining a simple source and sink did not work: 

```
joern> def sink = cpg.call("sys_unsafe_log_info").argument
def sink: Iterator[io.shiftleft.codepropertygraph.generated.nodes.Expression]
                                                                                                                                                                                        
joern> def source = cpg.call("open_input_files")
def source: Iterator[io.shiftleft.codepropertygraph.generated.nodes.Call]
                                                                                                                                                                                        
joern> sink.reachableByFlows(source).p
val res10: List[String] = List(
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
│MethodParameterIn │config_deserialize(FILE* stream)                                                                                │355 │config_deserialize  │configfile_deserialize.c│
│Identifier        │!stream                                                                                                         │356 │config_deserialize  │configfile_deserialize.c│
│Identifier        │parse_contents(main_block, stream, 0)                                                                           │365 │config_deserialize  │configfile_deserialize.c│
│MethodParameterIn │parse_contents(config_block* block, FILE* stream, int expect_closing_brace)                                     │324 │parse_contents      │configfile_deserialize.c│
│Identifier        │lex_token(stream)                                                                                               │326 │parse_contents      │configfile_deserialize.c│
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
└──────────────────┴────────────────────────────────────────────────────────────────────────────────────────────────────────────────┴────┴────────────────────┴────────────────────────┘""",
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
└──────────────────┴─────────────────────────────────────────────────────┴────┴──────────────────────────┴────────────┘"""
)
```

We decided to then change the solution path to traverse in reverse order from a `system()` call.

#### Making Joern a requirement

There were many things to consider to make the code resistant against simple code search.
Obfuscating file and variable names was a first step.
Additionally, we have put similar looking logging statements into the code. Some of them are vulnerable, but dead code,
Others are safe log functions, and others are only printing from buffers the user cannot control.
File size is also varying across the files, so we checked for similar sized files such that the vulnerable C file can't be easily
identified just by checking them.

We believe that a convoluted regex/grep code search across the files is still possible.
We have done our best to mitigate some of the easiest code search approaches.


## Conclusion & Future Work

```
What are the limitations of your current implementation?
If a future student were to pick up this project, what is the logical "next step"?
```

Logical next steps would be to add new vulnerabilities, where then one is randomly picked and add to the code.

Overall this project is a nice and interesting way to learn about the use of joern, the obfuscation of code and hidden configuration errors which should be considered and tested thorougly when pushing software to production.

## Generative AI (If Applicable)

```
Provide a brief summary of how generative tools actually influenced the project compared to your initial proposal.
```

Only minimally, it helped creating the style.css of our webservice to speed up development.

Schicho has also used GenAI for developing the C application. It was mainly used to ask questions, i.e. a replacement for StackOverflow.
GenAI was also used to debug the deserialization code as that one turned out to be the most tricky to implement.
