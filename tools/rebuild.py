import subprocess


def main():
    subprocess.run(["echo", "\n\r============remove files============\n\r"])
    subprocess.run(["scons", "-c"])
    subprocess.run(["python3", "./tools/build.py"])
    # subprocess.run(["arm-none-eabi-objdump", "-d",
    #                 "./rtthread-stm32f4xx.elf", ">./rtthread.s"])
    # subprocess.run(["make", "clean"])
    # subprocess.run(["make", "all"])
    # subprocess.run(["make", "change"])
    # subprocess.run(["arm-none-eabi-objdump", "-d",
    #                 "./OBJ/Output.elf", ">./OBJ/Output.s"])


if __name__ == "__main__":
    main()
