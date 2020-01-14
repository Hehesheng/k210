import subprocess


def main():
    subprocess.run(["scons", "--menuconfig"])
    # os.system("arm-none-eabi-objdump -d ./rtthread-stm32f4xx.elf >./rtthread.s")
    # os.system("/Users/huanghuisheng/.env/tools/scripts/pkgs ~/.env/env.sh")


if __name__ == "__main__":
    main()
