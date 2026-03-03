*This project has been created as part of the 42 curriculum by ayusa*

# Description
A program that solves the Dining Philosophers Problem using POSIX threads and mutexes.

Philosophers sit at a round table and alternate between eating, thinking, and sleeping. There is one fork placed on the left and right of each philosopher, and both forks are required to eat. The philosophers do not communicate with each other, and the simulation ends when someone starves to death.

**Rules:**
- Each philosopher operates as an independent thread.
- Forks are protected by mutexes to prevent concurrent access.
- Philosophers take only one of three states at a time: eating, thinking, or sleeping.
- If a philosopher does not start eating within `time_to_die` milliseconds, they die.
- The simulation ends when all philosophers have eaten `number_of_times_each_philosopher_must_eat` times (if specified).
- Data races must not occur.

**Log output format:**
```
timestamp_in_ms X has taken a fork
timestamp_in_ms X is eating
timestamp_in_ms X is sleeping
timestamp_in_ms X is thinking
timestamp_in_ms X died
```

# Instructions

**Compilation**
```
cd philo
make
```

**Execution**
```
./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
```

**Arguments**
`number_of_philosophers` : The number of philosophers (= number of forks)
`time_to_die` : Time limit (in milliseconds) from the start of the last meal to the start of the next meal. Exceeding this causes death.
`time_to_eat` : Time it takes for one meal (in milliseconds)
`time_to_sleep` : Time it takes for one sleep (in milliseconds)
`number_of_times_each_philosopher_must_eat` : The simulation ends when all philosophers have eaten this many times (optional)

**Execution examples**
```
./philo 5 800 200 200
./philo 4 410 200 200
./philo 5 800 200 200 7
./philo 1 800 200 200
```

**Cleanup**
```
make fclean
```

# Resources

- [「(outdated) philosopherとは何だったのか」 - Qiita](https://qiita.com/42yliu/items/86d16cdbc584c250ca6e)
- [食事する哲学者の問題 - Wikipedia](https://ja.wikipedia.org/wiki/%E9%A3%9F%E4%BA%8B%E3%81%99%E3%82%8B%E5%93%B2%E5%AD%A6%E8%80%85%E3%81%AE%E5%95%8F%E9%A1%8C)
- [pthread - Linux man pages](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- [gettimeofday(2) - Linux man pages](https://man7.org/linux/man-pages/man2/gettimeofday.2.html)

## AI Usage

In this project, AI (Claude code, Gemini) was used for the following tasks:
- Explanation of man pages and operational characteristics for each function (pthread_*, gettimeofday, printf).
- Cause analysis of the death detection problem related to the combination of log output and functions.
- Proposals for accuracy improvement methods to output the death message within time_to_die.
- Specification design of ft_usleep and explanation of busy-waiting methods.
- Explanation of the characteristics of printf and gettimeofday, along with methods for improving accuracy.
- Proposals for function naming conventions and file splitting design.
- Pre-review assuming peer review (norminette inspection and behavior testing).
- Proposals for validation design of command-line arguments.
- Explanation of the necessity of error branching and implementation policies.
- Review of this file.