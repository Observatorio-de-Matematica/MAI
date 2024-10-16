# № Отчет по лабораторной работе №4
## по курсу "Логическое программирование"

## Обработка естественного языка

### студент: Куликов А.В.

## Результат проверки

| Преподаватель     | Дата         |  Оценка       |
|-------------------|--------------|---------------|
| Сошников Д.В. |              |               |
| Левинская М.А.|              |               |

## Введение

Основные подходы к обработке естественных и искусственных языков:

- Символьный
- Статистический
- Коннективистский
- Гибридный

В данный момент нам более интересен символьный подход.

Символьный подход базируется на словарях и правилах, предварительно разработанных людьми.
Зачастую представителями символьного подхода являются системы, в основе которых лежат логика и правила. Символьная структура в таких системах представлена в форме логических высказываний.
Все преобразования(если они производятся) осуществляются по правилам вывода.
Такие системы обычно состоят из набора правил, механизма вывода и рабочей среды.
Знания представляются как множество фактов и правил в базе знаний.

Нетрудно заметить схожесть символьного подхода к обработке данных и процесса логического вывода языка Пролог.

К тому же во многих диалектах Пролога встроены средства для удобной работы с DCG-грамматиками (оператор `-->`). Работа данного оператара основана на разностных списках, которые гораздо производительнее более очевидных реализаций "откусывания" частей предложения с помощью `append`.

Дале будет показано, что Пролог действительно подходит для анализа текста на естественном языке.

## Задание(Вариант 10)

Реализовать разбор фраз языка, представляющих собой положительные и отрицательные высказывания. В результате предикат должен выдавать все атомарные глубинные структуры.

## Принцип решения

Предложение разбивается на части(терминалы и нетерминалы), каждая из которых сопоставляются словам в предложении. Разбиение происходит до тех пор пока не доберется до терминальных символов. Каждый(почти) предикат, обозначающий ту или иную часть предложения, имеет список аргументов, благодаря которым от более мелких частей информация "пробрасывается" более крупным вверх.

```prolog
% Предложение состоит из подлежащего и подпредложения (назовем это так)
sentence(X) --> obj(Y), sub(Y, X).
% Подпредложение состоит из группы глагола
sub(Y, X) --> verb_g(Y, X).
% либо из группы глагола, информация из которой нас интересует, и еще одного подпредложения
sub(Y, X) --> verb_g(Y, X), sub(_, _).
% либо из группы глагола, и еще одного подпредложения, информация из которого нас интересует
sub(Y, X) --> verb_g(_, _), sub(Y, X).
```

Здесь просто возможные структуры предложений. Так как нам нужно получить все атомарные структуры, которые могут быть разбросаны по всему предложению еще мы вынуждены использовать рекурсию. На каждом этапе доказательства мы получаем одну единственную глубинную структуру. Сначала головную, потом остальные в хвосте предложения. Если добрались до последнего подпредложения, то останавливаем доказательство.

```prolog
% группа глагола состоит из союза(опционально) и словосочетания с каким-то субъектом и сказуемого "любит"
verb_g(X, likes(X, Y)) --> (pr_g; []), likes(Y).
% либо из союза и сказуемого "не любит"
verb_g(X, not_likes(X, Y)) --> (pr_g; []), not_likes(Y).

% словосочетания с каким-то субъектом сказуемым "любит"
likes(X) --> ["любит"], subj_g(X).
% словосочетания с каким-то субъектом сказуемым "не любит"
not_likes(X) --> ne, ["любит"], subj_g(X).
```

Здесь и происходит выделение и классификация интересующих нас структур. Если в предложении есть частица "не", то возвращающий терм означает отрицание. Иначе говорится что подлежщее любиит что-то. Это что-то мы получаем при помощи правила `likes\3` или `not_likes\3`.

```prolog
% Обьект -- подлежащее
obj(X) --> [X].
% Субьект -- дополнение
subj(X) --> [X].

% группа субьектов состоит либо из субъекта, который нас интересует и субьекта, 
% который не интересует
subj_g(X) --> subj(X), ["и"], subj(_).
% либо из субьекта, который нас не интересует и субьекта, который интересует
subj_g(X) --> subj(_), ["и"], subj(X).
% либо из одиночного субьекта
subj_g(X) --> subj(X).
```

Для описания группы субъектов, которые может любить или не любить объект опредено правило `subj_g`. Оно описывает, что группа субьектов может быть определена как один субъект, и тогда вернется единственный вариант. Либо как два субъекта, которые будут возвращаться по очереди.

```prolog
% частица не
ne --> ["не"].
```

Просто частица "не".

```prolog
% Предикат для вычленения всех атомарных глубинных структур вида likes(X, Y) и not_likes(X, Y),
% где X -- объект, субьект, из предложения.
decompose(X, Y):-
    setof(Z, phrase(sentence(Z), X), Y).
```

Здесь получаем список уникальных атомарных структур, найденных в тексте.

## Результаты

```prolog
?- decompose(["Саша", "любит", "кубики", "и", "шарики"], X).
X = [likes("Саша", "кубики"), likes("Саша", "шарики")].

?- decompose(["Саша", "любит", "кубики", ",", "но", "не", "любит", "ролики"], X).
X = [likes("Саша", "кубики"), not_likes("Саша", "ролики")].

?- decompose(["Саша", "любит", "кубики", "и", "шарики", ",", "но", "не", "любит", "ролики", "и",
"квадратики",",", "а", "любит", "штуковины"], X).
X = [likes("Саша", "кубики"), likes("Саша", "шарики"), likes("Саша", "штуковины"), not_likes
("Саша", "квадратики"), not_likes("Саша", "ролики")].
```

## Выводы

Пролог оказался довольно удобным для решения задач граммматического разбора предложений, по крайней мере простейших его видов. Это обусловлено тем, что в используемом мной диалекте языка Пролог встроены удобные средства для работы с DCG-грамматиками. С помощью них предложение представляется как упорядоченный набор частей, с помощью которых довольно лаконично можно описать структуру предложений, а большего для выполнения задачи и не требовалось.

Итак, в результате работы над данной лабораторной получена программа, позволяющая по заданному предложению получить все атомарные структуры вида `likes`/`not_likes(объект, субьект)`, содержащиеся в нем. Знания полученные в ходе разработки данной программы могут быть перенесены на более сложные примеры, что, в принципе, может быть окажется полезным в будущем.