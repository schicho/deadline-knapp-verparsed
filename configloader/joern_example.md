# Joern Example

1. Start joern in the configloader directory.
2. import the code by typing into the joern shell:

```sc
importCode(inputPath=".", projectName="ctf")
```

3. define sink and source

```sc
def sink = cpg.call("sys_run")
def source = cpg.call("open_cfg_file") 
```


4. get reachableBy(Flows)

```sc
sink.reachableByFlows(source).p
```

Output:

```
joern> sink.reachableByFlows(source).p
val res5: List[String] = List(
  """
┌─────────────────┬───────────────────────────────────────┬────┬────────┬────────┐
│nodeType         │tracked                                │line│method  │file    │
├─────────────────┼───────────────────────────────────────┼────┼────────┼────────┤
│Call             │open_cfg_file("expl01t.txt")           │21  │main    │main.c  │
│Identifier       │* f = open_cfg_file("expl01t.txt")     │21  │main    │main.c  │
│Identifier       │f != NULL                              │22  │main    │main.c  │
│Identifier       │fread(buf, sizeof(char), sizeof buf, f)│23  │main    │main.c  │
│Identifier       │fread(buf, sizeof(char), sizeof buf, f)│23  │main    │main.c  │
│Identifier       │safe_run(buf)                          │27  │main    │main.c  │
│MethodParameterIn│safe_run(const char* cmd)              │12  │safe_run│main.c  │
│Identifier       │sys_run(cmd)                           │13  │safe_run│main.c  │
│MethodParameterIn│sys_run(const char* cmd)               │7   │sys_run │system.c│
│Identifier       │system(cmd)                            │8   │sys_run │system.c│
│Call             │system(cmd)                            │8   │sys_run │system.c│
│Return           │return system(cmd);                    │8   │sys_run │system.c│
│MethodReturn     │RET                                    │7   │sys_run │system.c│
│Call             │sys_run(cmd)                           │13  │safe_run│main.c  │
└─────────────────┴───────────────────────────────────────┴────┴────────┴────────┘"""
)

```
