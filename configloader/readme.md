# configloader development notes

Wir wrappen einfach mal alles unnötig oft.

- `printer.c`: wrapped ganz normal die fprint funktionen aus stdio.h
- `logger.c`: logging funktionalität die gleich unsere print funktionen verwendet
- `filesystem.c`: high-level funktionen um Datein zu öffnen. Darf gerne noch erweitert werden. Benutzt intern `fs_low`.
- `fs_low.c`: öffnet die Datein eigentlich mittels `fsops.h`. macht random sanity checks.
- `fsops.h`: wrapped die fopen, etc. Funktionen.
- `system.h`: wrapped `system()` und `popen` calls. **Irgendwie wollen wir das obfuscaten**.
- `sysinfo.c`: safe uninjectable functions that call functions in `system.h`.
- `sysunsafe.c`: test-weise da um die ausführung von befehlen zu ermöglichen.

## building

`make` und `make clean` auf normal halt. Idk ob es mit windows geht. benutzt das wer von uns?