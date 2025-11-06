def read_proc_file():
    with open("/proc/ldd_demo_driver", "r") as file:
        content = file.read()
        print(content)
        file.close()
    return 0

if __name__ == "__main__":
    read_proc_file()