# mini_serv

This is how I went through understanding, learning, and practicing for the *42 School* last exam (Rank 06), `mini_serv`.

`mini_serv` is a tiny implementation of a TCP chat server written in only one C file.

The server listens on `127.0.0.1` `(localhost)`. It uses`select()` to handle clients, it sets successive client IDs, and it relays messages between them without blocking on individual clients.
## Exam setup

As with all *42 School* exams, we're presented with two directories under `~`: `~/subject` and `~/rendu`.

`~/subject` contains the subject and the starter code, `main.c`:

```text
subject/
└── mini_serv/
    ├── en.subject.txt
    └── main.c
```

`~/rendu` is the Git repository to be evaluated. It comes empty, so *Step 1* is to create the subdirectory `mini_serv` and copy `~/subject/mini_serv/main.c` into it as `~/rendu/mini_serv/mini_serv.c`.

**From `rendu`:**

```zsh
mkdir mini_serv && cp ../subject/mini_serv/main.c mini_serv/mini_serv.c
```

This results in:

```text
rendu/
└── mini_serv/
    └── mini_serv.c
```

## Repository structure

I'm adding a third directory, `solution/`, to this repository. It contains the completed implementation and all the files I used to learn and practice the solution step by step.

The repository tree:

```text
.
├── README.md
├── .gitignore
├── subject/
│   └── mini_serv/
│       ├── en.subject.txt
│       └── main.c
├── rendu/
│   └── mini_serv/
│       └── mini_serv.c
└── solution/
    ├── mini_serv_reference.c
    ├── steps/
    │   ├── 00_starter_setup.c
    │   ├── 01_args.c
    │   ├── 02_fatal_errors.c
    │   ├── 03_select_setup.c
    │   ├── 04_accept_client.c
    │   ├── 05_arrival_broadcast.c
    │   ├── 06_receive_data.c
    │   ├── 07_extract_lines.c
    │   ├── 08_broadcast_messages.c
    │   ├── 09_partial_send.c
    │   └── 10_disconnect_cleanup.c
    └── helpers/
        ├── 00_extract_message.c
        ├── 00_str_join.c
        ├── 01_print_fatal.c
        ├── 02_msg_to_all.c
        ├── 03_msg_client_arrived.c
        ├── 04_prefix_line_loop.c
        ├── 05_msg_client_left_clr.c
        ├── 06_server_recv_msgs.c
        └── 07_server_send_msgs.c
```

## Solution, step by step

Each C file in `solution/steps/` is cumulative, building from the starter code in `subject/mini_serv/main.c` toward the completed implementation in `solution/mini_serv_reference.c`.

Each step can be copied over the current working file while learning or practicing. For example, from the root of the repository:

```zsh
cp solution/steps/05_arrival_broadcast.c rendu/mini_serv/mini_serv.c
```

I recommend always compiling and testing the current working file after manually working through a step before moving on to the next one.

The earlier steps are intentionally incomplete and only implement what has been introduced up to that point.

**The steps:**

```text
00  starter setup
01  argument handling
02  fatal errors
03  select() and fd sets
04  accepting and registering clients
05  arrival messages and broadcasting
06  receiving data
07  extracting complete lines
08  prefixing and broadcasting client messages
09  queued output, partial send() and MSG_NOSIGNAL
10  disconnect handling and cleanup
```

## Helper reference

I'm also adding a `solution/helpers/` directory with each helper function in a separate file, including the first two provided by the starter code, for quick reference.

## Compile and run

I didn't add a Makefile because compiling `mini_serv.c` directly from the terminal better simulates the exam and makes the compiler command part of the practice.

The untouched starter `main.c` does not compile cleanly with these flags. `00_starter_setup.c` is the first step that does.

**From the root of the repository:**

```zsh
cc -Wall -Wextra -Werror rendu/mini_serv/mini_serv.c -o mini_serv
```

Then run it:

```zsh
./mini_serv 8041
```

The completed reference file can also be compiled so you always have the final working program to compare your current implementation against:

```zsh
cc -Wall -Wextra -Werror solution/mini_serv_reference.c -o mini_serv_reference
```

If you want to run both at the same time, use a different port for the reference:

```zsh
./mini_serv_reference 8042
```

Then test `mini_serv` with Netcat from other terminals. Each `nc` connection acts as a new client:

```zsh
nc 127.0.0.1 8041
```

or:

```zsh
nc localhost 8041
```

To connect to the reference server running on port `8042` instead:

```zsh
nc localhost 8042
```

## Submission

Remember that on the exam you'll need to check that only the `mini_serv.c` file is present in `~/rendu/mini_serv`, so delete any compiled files from the directory before submitting.

In the exam, remember:

```zsh
cd ~/rendu
git add mini_serv/mini_serv.c
git commit -m "good luck!"
git push
```