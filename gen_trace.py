import random

with open("trace.txt", "w", encoding="utf-8") as traceFile:
    for i in range(100):
        gap = random.random()
        ddl = random.randrange(0, 0x5265C00)
        prio = random.randrange(0, 10000)
        blk = random.randrange(0, 0xFFFF)
        traceFile.write(f"{gap}    {ddl}    {blk}    {prio}\n")