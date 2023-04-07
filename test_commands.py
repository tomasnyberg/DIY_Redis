import subprocess
import concurrent.futures
import random


def generate_random_commands(n):
    get_set_del = ["get", "set", "del"]
    commands = []
    for _ in range(n):
        cmd = ['make', 'run-client']
        argstring = 'ARGS='
        chosencommand = random.choice(get_set_del)
        argstring += chosencommand
        argstring += " "
        argstring += str(random.randint(0, 100))
        if chosencommand == "set":
            argstring += " " + str(random.randint(0, 100))
        argstring += ''
        cmd.append(argstring)
        commands.append(cmd)
    return commands


def run_command(cmd):
    result = subprocess.run(cmd, stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE)
    return result.stdout.decode('utf-8')


with concurrent.futures.ThreadPoolExecutor(max_workers=5) as executor:
    # Submit each command to the executor and store the future object in a list
    futures = [executor.submit(run_command, command)
               for command in generate_random_commands(10)]

    # Wait for all futures to complete and print their output
    for future in concurrent.futures.as_completed(futures):
        print(future.result())
