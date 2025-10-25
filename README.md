# Lock-Free Очереди на C++20  
**Высокопроизводительная неблокирующая передача данных между потоками — реализовано с нуля.**

---

## Обзор

Проект содержит две реализации конкурентных очередей без блокировок:

| Тип очереди | Модель | Описание |
|--------------|---------|-----------|
| **RingBuffer** | *Single Producer, Single Consumer* | Сверхбыстрая кольцевая очередь для одного производителя и одного потребителя. |
| **MPMCqueue** | *Multi Producer, Multi Consumer* | Многопоточная lock-free очередь по алгоритму Дмитрия Вьюкова с номерами поколений для каждой ячейки. |

Обе очереди реализованы **на современном C++20** с использованием атомарных операций и строгих memory order.

---

## Структура проекта

```
lock_free_queue
 ┣ include
 ┃ ┣ RingBuffer.hpp
 ┃ ┗ MPMCqueue.hpp
 ┣ src
 ┃ ┣ RingBuffer.cpp
 ┃ ┗ MPMCqueue.cpp
 ┣ tests
 ┃ ┗ test_queue.cpp
 ┣ benchmarks
 ┃ ┗ benchmark_queue.cpp
 ┣ CMakeLists.txt
 ┗ README.md
```

---

## Сборка и запуск

### 1 Требования
- CMake ≥ 3.15  
- Компилятор C++20 (g++, clang++, MSVC)  
- [GoogleTest](https://github.com/google/googletest)  
- [Google Benchmark](https://github.com/google/benchmark)  

### 2 Сборка всех целей

```bash
mkdir build && cd build
cmake ..
make -j
```

### 3 Запуск юнит-тестов

```bash
./test_queue
```

Вывода:
```
[==========] Running 6 tests from 2 test suites.
[----------] Global test environment set-up.
[----------] 3 tests from RingBuffer
[ RUN      ] RingBuffer.SingleThreadPushPop
[       OK ] RingBuffer.SingleThreadPushPop (0 ms)
[ RUN      ] RingBuffer.MultiPushSinglePop
[       OK ] RingBuffer.MultiPushSinglePop (0 ms)
[ RUN      ] RingBuffer.ConcurrentProducerConsumer
[       OK ] RingBuffer.ConcurrentProducerConsumer (63 ms)
[----------] 3 tests from RingBuffer (63 ms total)

[----------] 3 tests from MPMCqueue
[ RUN      ] MPMCqueue.SingleThreadPushPop
[       OK ] MPMCqueue.SingleThreadPushPop (0 ms)
[ RUN      ] MPMCqueue.MultiProducerMultiConsumer
[       OK ] MPMCqueue.MultiProducerMultiConsumer (240 ms)
[ RUN      ] MPMCqueue.StressTest
[       OK ] MPMCqueue.StressTest (49 ms)
[----------] 3 tests from MPMCqueue (289 ms total)

[----------] Global test environment tear-down
[==========] 6 tests from 2 test suites ran. (353 ms total)
[  PASSED  ] 6 tests.
```

---

## Бенчмарки

### Запуск тестов производительности

```bash
make benchmark_queue
./benchmark_queue
```

### Пример вывода

```
----------------------------------------------------------------------------------------------
Benchmark                                    Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------------
BM_RingBuffer_PushPop/iterations:15   33777549 ns       186391 ns           15 items_per_second=1.07301G/s
BM_MPMC_PushPop/iterations:15         65193587 ns       438052 ns           15 items_per_second=913.134M/s
```

**Интерпретация:**
- `RingBuffer` выполняет >1 млн операций в секунду.
- `MPMCqueue` масштабируется на несколько потоков и остаётся полностью lock-free.

---

## Краткая теория

### RingBuffer
- Два атомарных индекса: `_head` и `_tail`  
- Без мьютексов, без CAS — только атомарные инкременты  
- FIFO, фиксированный размер  
- Задержка — единицы наносекунд

### MPMCqueue
- Основана на алгоритме **Dmitry Vyukov MPMC bounded queue**  
- Каждая ячейка хранит `sequence` — номер поколения  
- CAS используется только для `_head` и `_tail`  
- Полностью lock-free и масштабируемая

#### Жизненный цикл ячейки
```
Producer: sequence == head       → можно писать
Consumer: sequence == tail + 1   → можно читать
После pop: sequence = tail + N   → ячейка снова свободна
```

---

## Тестирование

Все тесты написаны с использованием **Google Test**.

| Набор | Тесты | Назначение |
|--------|--------|-------------|
| `RingBuffer` | `SingleThreadPushPop`, `MultiPushSinglePop`, `ConcurrentProducerConsumer` | Проверка корректности и FIFO |
| `MPMCqueue` | `SingleThreadPushPop`, `MultiProducerMultiConsumer`, `StressTest` | Проверка потокобезопасности и пропускной способности |

Запуск всех тестов:
```bash
ctest
```
### Пример вывода

```
    Start 1: LockFreeTests
1/1 Test #1: LockFreeTests ....................   Passed    0.39 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.40 sec
```
---

## Производительность

| Очередь | Сценарий | Потоки | Операций/с |
|----------|------------|----------|--------------|
| **RingBuffer** | 1 продюсер + 1 консюмер | 2 | ~1 млн |
| **MPMCqueue** | 4 продюсера + 4 консюмера | 8 | ~8–9 млн |

---

## Используемые технологии
- **C++20 STL atomics**  
- **CAS (Compare-And-Swap)**  
- **GoogleTest** — тестирование  
- **Google Benchmark** — профилирование

---

## Лицензия

Проект распространяется под лицензией **MIT**.  
© 2025 — разработано для обучения и демонстрации принципов lock-free программирования.