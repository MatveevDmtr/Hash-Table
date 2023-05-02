# "Hash Table"
## Project description
The project's main aim is to test different hash functions and to optimize the process of searching words in a hash table. Let's make a research, how changing hash function and different optimizations influence on running time and other characteristics of our program.
## Some theory
A hash table is a data structure that implements an associative array or dictionary. It is an abstract data type that maps keys to values. A hash table uses a hash function to compute an index, also called a hash code, into an array of buckets or slots, from which the desired value can be found. 

The picture below can help you to study how a hash table should be arranged:
![Working principle](./img/hashtable1.png)

For more information about hash tables follow https://en.wikipedia.org/wiki/Hash_table.

## Work plan
Let's create a hash table that contains single linked lists. As you can see, C++ has been chosen as the main language for realization of our plans.
Our program should have an oportunity to use different hash functions, so it's advisable to get such function as an argument.
Then we should fill the table with a great amount of words. I've chosen "Fahrengeit 451" by Ray Bradburry as a text to get words from.

##### Part 1. Testing hash functions
Let's create diagrams that show list sizes for each hash value. Ideal hash function should have a flat diagram, so we are going to compare the results and make a kind of rating of hash functions.

##### Part 2
This part of the project involves creating different manual optimizations. Here is the list of optimizations we are going to use:
1. Assembly insertions. We are going to write a function in NASM and call it from C++. Documentation for NASM can be found here https://www.opennet.ru/docs/RUS/nasm/.
2. AVX (Advanced Vector Extensions, extension of Intel & AMD command system). To learn more about AVX follow https://www.laruence.com/sse/.
3. _______Bla Bla Bla_______

A block of code to optimize will be chosen using FlameGraph - an instrument that allows to find the most slow and "heavy" functions in our program.
As a result, let's compare running time of our program before and after inserting optimizations.

## Part 1. Testing hash functions

#### Program features

Let's collect some statistics. It's advisable to create a function for saving all list sizes to csv files with appropriate names. Then, using LibreOffice or MS Excel, we should create diagrams with info from this file.

Some words about size of the hash table. In real conditions hash tables have 1-2 elements in each list to get a better performance. These conditions cause a huge size of the hash table. But in case of testing hash functions 1-2 elements in each list will spoil our diagrams and make them uninformative. That's why we should intensionally decrease hash table's size to make diagrams better.

Also, theory of hashing says, that size of a hash table should be a prime number to get a better distribution. It's obvious that this size depends on amount of words in the text. Let's choose a divisor for number of words in the text:
$$hashtable \text{ } size = \frac{number \text{ } of \text{ } words \text{ } in \text{ } the \text{ }text}{divisor}$$
These conditions should be satisfied:
1. Size of the hash table should be a prime number;
2. For a rather good hash function there should be 10-20 words in each list.
   
I experimentally obtained, that this divisor should have a value _99_ for the chosen text.

So, let's start describing and analyzing each hash function.

#### Hash function #1. "Always 1" [Always the first]
It's a basic hash function that always returns _1_ regardless of the word. Here is its implementation:
```
size_t Hash_Always1(const char* word)
{
    return 1;
}
```
Diagram:
![Always1](./img/hash_always1.png)

As you can see, all the words have been stored to a list with index _[1]_. This function has a diagram that is as far as possible from a flat one.
Actually, __"Always 1"__ is a simple test for the algorythm of hashing, but not a function for real use.

#### Hash function #2. "First ASCII"
This hash function returns ASCII code of the first letter of a word. Here is its implementation:
```
size_t Hash_FirstASCII(const char* word)
{
    return word[0];
}
```
Diagram:
![first_ascii](./img/hash_firstASCII.png)

The diagram has 2 very high columns (414 words for hash value _99_ and 672 words for hash value _115_) and a small range of hash values (from 67 to 122).
Hash tables based on __"First ASCII"__ function are usable, but extremely inefficent.

#### Hash function #3. "Strlen"
This hash function returns length of a word as a hash value. Here is its implementation:
```
size_t Hash_Strlen(const char* word)
{
    return strlen(word);
}
```
Diagram:
![strlen](./img/hash_strlen.png)

The diagram actually reminds some kind of a _normal_ _distribution_. All the words have a hash value not bigger than 16. This range (from 0 to 16) is increadibly small for a good hash function. Though the diagram is far from a flat one, __"Strlen"__ is already a possible variant for creating a working hash table.

#### Hash function #4. "Sum ASCII codes"
This hash function returns sum of ASCII codes of all symbols in a word. Here is its implementation:
```
size_t Hash_SumASCII(const char* word)
{
    size_t sum = 0;
    int i = 0;
    while (word[i] != '\0')
    {
        sum += word[i];
        i++;
    }
    return sum;
}
```
Diagram:
![sumASCII](./img/hash_sumASCII.png) 

The first advantage of this function, that we see in the diagram - it uses all hash value range (unlike other 3 hash functions that we have already tested). I must admit, that it's because of our intensional decreasing of hash value range. In real conditions this hash function will hardly use full hash value range. Sum of ASCII codes is limited by multiplication of max ASCII code of a letter to max number of letters in a word. Hash value range can be much bigger than the result of this multiplication.
However, the diagram has 4 pronounced peaks, and some lists have up to 29 elements. __"Sum ASCII codes"__ function looks rather attractive in comparison with __"Always 1"__, __"First ASCII"__ and __"Strlen"__, but it still has a lot of disadvantages.

#### Hash function #5. "ROL"
This hash function counts the hash value using this formula:
$$H_{i+1} = rol(H_i) \wedge word[i]$$
As a start hash value we take $H_0 = 0$.
$rol$ is an assembly function that implements bit rotation to the left.
It works like this:
```
size_t Hash_ROL(const char* word)
{
    size_t value = 0;
    int i = 0;
    while (word[i] != '\0')
    {
        value = ((value << 1) | (value >> (sizeof (int) - 1))) ^ word[i];
        i++;
    }
    return value;
}
```
Diagram:
![rol](./img/hash_rol.png)

This function uses all hash value range too. Though the diagram has very high columns, most of them are outstanding. Generally, the values are closer to the average one, than in functions described before.

#### Hash function #6. "ROR"
This hash function is similar to the previous one, but $rol$ changes to $ror$:
$$H_{i+1} = ror(H_i) \wedge word[i]$$
As a start hash value we take $H_0 = 0$.
$ror$ is an assembly function that implements bit rotation to the right.
It works like this:
```
size_t Hash_ROR(const char* word)
{
    size_t value = 0;
    int i = 0;
    while (word[i] != '\0')
    {
        value = ((value >> 1) | (value << (sizeof (int) - 1))) ^ word[i];
        i++;
    }
    return value;
}
```
Diagram:
![ror](./img/hash_ror.png)

This function uses all hash value range too. Though the diagram has very high columns, most of them are outstanding. Generally, the values are closer to the average one, than in functions described before.

#### Hash function #7. "RS"
I've chosen this hash function, because it has rather good characteristics and a very simple implementation. 
It works like this:
```
size_t Hash_Rs(const char* word)
{

    static const unsigned int b = 378551;
    unsigned int a = 63689;
    size_t value = 0;

    int i = 0;
    while (word[i] != '\0')
    {
        value = value * a + (unsigned char) word[i];
        a *= b;
        i++;
    }

    return value;
}
```
Diagram:
![rs](./img/hash_ror.png)

This function uses all hash value range too. Though the diagram has very high columns, most of them are outstanding. Generally, the values are closer to the average one, than in functions described before.

## Автоматическая сборка
Чтобы запустить программу, необходимо использовать Makefile, прилагающийся к проекту. Для этого после клонирования этого репозитория на ваш компьютер  необходимо в теминале из папки репозитория набрать "make". В самом Makefile в переменной CFLAGS необходимо указать нужный тип оптимизации (см. ниже). Используя флаг условной компиляции -DDRAW  (см. Makefile) можно включать и выключать отрисовку множества, а с помощью флага -DMEASURING можно входить и выходить из режима измерений.

## Сборка вручную
Для компиляции программы необходимо прописать в терминале следующую команду:

```g++ -mavx2 -c -O3 -DDRAW mandelbrot.cpp -o mandelbrot.o```

Сборка проекта:

```g++ mandelbrot.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system```

Чтобы запустить программу, пропишите в терминале:

```./sfml-app```

## Part 2. Optimizations

#### Tools for analysis
There is a great amount of various tools and applications that can analyze your program at runtime. I've chosen callgrind (an utility that is served in a valgrind package) for my research. It emulates each executable program instruction. Callgrind uses internal metrics about the «cost» of each instruction to give us the conclusion we need.
To install valgrind (that contains callgrind), type:

```sudo apt install valgrind```

We also need a tool that will help us to read and analyze Callgrind's reports. Kcachegrind is maybe one of the most useful programs for this case.
For more information about valgrind see its [official site](https://valgrind.org/), [quick start guide](https://valgrind.org/docs/manual/quick-start.html) and [user manual](https://valgrind.org/docs/manual/manual.html).

#### Word search implemetation
Probably the most important function in a hash table is a function that searches words. In my program it's rather simply implemented:

```
int SearchWord(htab_t* hashtable, const char* word)
{
    size_t index = hashtable->HashFunc(word) % hashtable->size;

    int res = SearchInList(&(hashtable->table[index]), word);
    return res;
}
```
A few words about searching in a list. In real hash tables lists don't usually contain more than 1-2 words, so such search doesn't take much time. That's why the most common idea is to organize a linear search. It can be done by "sign by sign comparison" of each word with the search word. 

But in our test case there can be more than 40 words in a list. It really depends on the hash function that we are going to use. So linear search can be rather slow in our conditions and maybe we should optimize it.

Here is the implementation of searching a word in a list:

```
int SearchInList(list_t* list, const char* word)
{
    node_t* node = list->head;

    for (size_t list_i = 0; list_i < list->size; list_i++)
    {
        if (!strcmp(node->elem, word))
        {
            // Word found!
            return list_i;
        }
        node = node->next;
    }

    // Word not found    
    return -1;
}
```

## Как достичь максимальной скорости вычислений? 

Проведем измерения FPS (количество кадров в секунду, что соответствует количеству рассчетов целого множества в секунду). Для повышения точности измерений будем усреднять значение FPS за первые 200 рассчетов множества. После 200-ой итерации в консоль выводится надпись "end of measuring". Значит, можно закрывать окно и считывать усредненное значение FPS, которое так же будет выведено в консоль сразу после закрытия окна.
Число 200 выбрано экспериментально по соотношению "время тестирования-точность результата". При желании в коде можно изменять число измерений (константа NUM_MEASURES).
В процессе измерений важно не двигать мышкой, не нажимать кнопки клавиатуры и не нагружать процессор другими способами. Эти сторонние факторы могут исказить результаты измерений.
Для упрощения измерений запустим функции с рассчета с AVX и без AVX последовательно друг за другом, вычисляя FPS отдельно для каждой функции.

### С отрисовкой


| Флаг оптимизации |       AVX2, fps        |    Без AVX2, fps    | Ускорение, раз |
| :------:         | :---------------: | :------------: | :------------: |
|    none          |       13.3        |       4.4      |       3.0      |
|    -O3           |       35.4        |       8.4      |       4.2      |
|   -Ofast         |       37.2        |       8.6      |       4.3      |

В графе "Ускорение" указано, во сколько раз использование AVX ускорило расчет по сравнению с одиночными вычислениями $\textit{при неизменном флаге компиляторной оптимизации}$.

Как мы видим, ускорение вычислений оставляет желать лучшего. Значительную часть времени работы программы составляет время, которое библиотека SFML тратит на отрисовку множества. Программно уберем отрисовку (сделаем пустое окно) и посмотрим, какого ускорения мы сможем достичь в этом случае. 

### Без отрисовки
Проведем замеры FPS для разных оптимизаций при отключенной отрисовке. 
| Флаг оптимизации |       AVX2, fps        |    Без AVX2, fps    | Ускорение, раз |
| :------:         | :---------------: | :------------: | :------------: |
|       none       |       15.6        |       4.6      |       3.4      |
|       -O1        |       57.9        |       8.9      |       6.5      |
|       -O2        |       60.0        |       9.0      |       6.7      |
|       -O3        |       58.6        |       9.1      |       6.4      |
|       -Ofast     |       64.2        |       9.4      |       6.8      |

В графе "Ускорение" указано, во сколько раз использование AVX ускорило расчет по сравнению с одиночными вычислениями $\textit{при неизменном флаге компиляторной оптимизации}$.

Отключение отрисовки существенно сказалось на времени работы программы. При малых FPS влияние отрисовки мало: 4.4 fps превратилось в 4.6 fps, ускорение в 1.05 раза. При высоких скоростях вычислений отрисовка занимает около 42% времени, что позволяет увеличить fps с 37.2 до 64.2 при её отключении.

## Сравнение результатов и выводы
Итого, мы получили, что использование AVX2 позволяет сократить время работы программы в 3-7 раз (в зависимости от типа компиляторной оптимизации). Таким образом, AVX2 в некоторых случаях может "обогнать" по времени компиляторные оптимизации, примененные к стандартному алгоритму. Одним из примеров объемных вычислений, для которых AVX-команды наиболее эффективны, и является множество Мандельброта, которое было исследовано в данном проекте.

Также необходимо отметить, что результаты измерений fps сильно зависят от многих факторов: от типа и модели процессора, от его загруженности и даже температуры. Результаты, представленные в работе, соответствуют запуску программы на процессоре Intel Core i5 при использовании Linux Mint 21. Все результаты получены при соблюдении практически одинаковых внешних условий. Однако, например, при других условиях запуска при максимальном использовании оптимизаций удается получить до 82 fps. Результаты измерений могут количественно отличаться в зависимости от условий запуска программы, но все качественные зависимости и тенденции, отмеченные в работе, сохранятся.