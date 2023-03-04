import argparse
import random

random.seed(1337)
INTS_FILE = "test/ints.txt"
RECORDS_FILE = "test/records.txt"

def generate_ints(num: int) -> None:
    with open(INTS_FILE, "w") as f:
        for _ in range(num):
            print(random.randint(-100_000, 100_000), file=f)

def generate_records(num: int) -> None:
    with open(RECORDS_FILE, "w") as f:
        for i in range(num):
            n = random.randint(1, 100)
            v = random.uniform(1, 100)
            print(f"{n} {v:.3f}", file=f)
            print(f"record_{i}", file=f)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-ints', '--ints_count',
                        type=int, default=100)
    parser.add_argument('-records', '--records_count',
                        type=int, default=10)
    args = parser.parse_args()
    generate_ints(args.ints_count)
    generate_records(args.records_count)

if __name__ == "__main__":
    main()
