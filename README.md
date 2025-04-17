# Linux Signal‑Driven Calculator

A tiny **client/server** pair in POSIX C that turns one process into a calculator daemon and lets any number of other processes request arithmetic over **UNIX signals + temp files**.

---

## ✨ What’s inside?

| File / Routine            | Purpose                                                         |
|---------------------------|-----------------------------------------------------------------|
| `server.c`                | Waits on `SIGUSR1`, reads `to_srv.txt`, performs + − × ÷, writes `response_client_<PID>.txt`, then notifies the caller with `SIGUSR1` (or `SIGUSR2` on bad op). |
| `client.c`                | Sleeps a random 5‑10 s to simulate concurrency, writes PID/operands/op‑code into `to_srv.txt`, signals the server, arms a 15 s `alarm()` timeout, and blocks on `pause()` until the answer arrives. |
| `loadLinesToBuffer()`     | Minimal dynamic line reader used by both programs              |
| `handleError()`           | perror/exit wrapper for every sys‑call                          |

---

## 📝 Customising

* **Operations** – in `server.c`, edit the `switch(operation)` to add more math.  
* **Timeout** – change `alarm(15)` in `client.c` to tighten or relax server‑response limits.  
* **Temp filenames** – update the `"to_srv.txt"` and `response_client_%d.txt` literals if you prefer named pipes or other paths.

---

## ⚖️ License

MIT — free to use, hack, and share.
