# Improvement Recommendations

## Highest Value

- Decide whether hostnames should be supported. If yes, update endpoint parsing and socket address resolution so `localhost:5000` passes intentionally.
- Add integration tests that start both chat peers, send a message, and verify the opposite peer receives it.
- Add a `--help` option for both executables.
- Add a graceful shutdown path for the receiver thread that does not depend on closing the socket from another thread.

## Testing

- Split parser tests into separate Catch2 test cases for ports, endpoints, and full command-line config.
- Add tests for empty messages, very long messages, and maximum UDP payload handling.
- Add CI instructions for running `mingw32-make test` on commit.

## Networking

- Add optional TCP mode if message delivery must be guaranteed.
- Add hostname support with `getaddrinfo`.
- Add clearer errors when a port is already in use.

## Maintainability

- Consider a `docs/` folder if the design notes grow.
- Keep generated binaries out of git; rebuild with `mingw32-make`.
- If Catch2 is shared by multiple AI comparison folders, consider documenting the vendoring policy at the repo level.
