# 100_Prisoners_Problem
This is my solution to my Operating System Assignment 4: the famous 100 prisoners problem(but multi-threaded in this case).

The 100 prisoners problem is stated by Philippe Flajolet and Robert Sedgewick as follows:
The director of a prison offers 100 death row prisoners, who are numbered from 1
to 100, a last chance. A room contains a cupboard with 100 drawers. The director
randomly puts one prisoner’s number in each closed drawer. The prisoners enter the
room, one after another. Each prisoner may open and look into 50 drawers in any
order. The drawers are closed again afterwards. If, during this search, every prisoner
finds his number in one of the drawers, all prisoners are pardoned. If just one prisoner
does not find his number, all prisoners die. Before the first prisoner enters the room,
the prisoners may discuss strategy — but may not communicate once the first prisoner
enters to look in the drawers. What is the prisoners’ best strategy?


A simple strategy is that every prisoner randomly chooses drawers while searching for his number.
This strategy is not very effective since every prisoner has a 50% chance to find his number. This
means that 100 prisoners have a chance of 0.5^100 to all find their numbers, which is almost zero.


A smart strategy is that every prisoner starts by opening the drawer with his own number. If the
number in the drawer matches, he has been successful. If not, he next opens the drawer with
the number found in the current drawer, i.e., the prisoner follows a pre-determined sequence of
drawers to find his number. This strategy provides a surprisingly high chance for all 100 prisoners
to find their numbers.

Your task is to simulate this game, implementing a C program. 

The full question decription can be found here https://cnds.jacobs-university.de/courses/os-2021/p04.pdf

PS.: Constructive review accommodated.

