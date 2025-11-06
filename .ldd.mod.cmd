savedcmd_ldd.mod := printf '%s\n'   ldd.o | awk '!x[$$0]++ { print("./"$$0) }' > ldd.mod
