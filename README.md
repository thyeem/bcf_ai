BCF Game and its AI gaming
===

## What

![dsfsdf](media/demo_neo.gif)

BCF stands for **B**itten **C**onnect-**F**ive and I created this game — in fact, just a variation on a East-Asian board game, "Gomoku" or "Five-in-a-row" — to make [Gomoku](https://en.wikipedia.org/wiki/Gomoku) a more complex rule. (In reality, sometimes Gomoku make us feel a little boring compared to its complexity of the game tree.) I just wanted a game that people could learn and enjoy time easily but not spending much time like [Go](https://en.wikipedia.org/wiki/Go_(game)).

I named it BCF, but it is actually closer to [Gomoku](https://en.wikipedia.org/wiki/Gomoku) rather than [Connect Five](https://en.wikipedia.org/wiki/Connect_Four). I just added some basic rules into the Gomoku so that I made it more complicated.



## How to build 

```bash
$ git clone https://github.com/thyeem/bcf_ai
$ cd bcf_ai

# there's almost no dependencies on other libs. 
# Here, 'ncurses' is the only external library used.

$ make
$ ./bcf_ai

# or simply build and run at the same time
$ make run
```



## Rule

In [Gomoku](https://en.wikipedia.org/wiki/Gomoku), a player who firstly makes five stones in a row in any direction wins. (not broken chain of five stones in horizontal, vertical, or diagonal direction) In addition to this, I defined two simple basic rules below:

* If your last move can clamp your opponent's *two successive* stones, you can capture them. Multi-directional capturing is possible with a move so theretically, up to 16 captures are possible with only a move.

* 10 points (10 captured stones) are equivalent to "the five stone in a row", meaning firstly winning 10 points is a victory. 



## BCF AIs: Sofiai and Mariai

I implemented two game AIs for BCF, called **Sofiai** and **Mariai**. Very weird name, but it originated from my two daughters, Sofia and Maria. (Sofiai = Sofia + AI, and Mariai = Maria + AI. Blame my naming skills :$)

For now, my second princess Maria is much, much stronger, but that does not mean I love her more. ;-)

I'd like to highlight a few backgrounds of **Sofiai** and **Mariai**.


* [Minimax algorithm](https://en.wikipedia.org/wiki/Minimax) was used in **Sofiai**. By default, Sofiai tries to search only 4-depth (4 plies). It's because that the ponding time would grow exponentially if we increase the depth of searching tree a little. With `~30` of branching factor, the depth of this level is the threshold of a typical PC.
* [MTCS (Monte-Carlo Tree Search)](https://en.wikipedia.org/wiki/Monte_Carlo_tree_search) and [UCT(Upper Confidence Bound applied to trees)](https://www.chessprogramming.org/UCT) method was used in **Mariai**. 
  That is, simply, each step of MCTS *1)select*, *2)expand*, *3)play-out (simulation)*, *4)update (backpropagation)* of the game tree is governed by the exploitation(doing more searches promising, *Q-value*) and the exploration(doing searches not frequently explored, *visit count N*).
* With the full tree search of 4-depth, **Sofiai** is quite fast and responsive, coping well with near futures, but not looking far ahead like **Mariai**.
* Right after Mariai's move, the *EWP(Estimated Winning Probability)* is updated. It indicates Mariai's odds, winning probability obtained by the most promising move (the most visited node in the game tree) throughout the play-outs. If Mariai's overall situation becomes better after a move, it will be increasing, otherwise, it will be dropped.
  

All the configurable parameters are defined in `common.h` such as *UCB constants*, *self-playout number* and *node expansion threshold* when searching trees at expansion stage (refer to `common.h`)

One can even change the given *board size (default: 19x19)*, *winning point (default: 10)*, and *BCF mode (default: 1)*. Those are because I was very curious how much it would affect the winning probabilities among the AIs from each different algorithm. 

Turning on "*BCF mode*" means literally to use the definition of BCF gaming rule. If you turn off the flag, you can enjoy game with the original rule, Gomoku!

When entering the game, one can access each gaming mode with shortcuts (`1`,` 2`, .. and `q`) as shown below.

![game intro](media/bcf_game.png)



## Todo

When playing with Mariai, long ponding time due to 1-million playouts is a little bit annoying (moreover, I did not introduce any parallel MCTS). One of big problems when searching BCF game tress is the huge *branching factor(b)* of `~20` and *average game length(d)* of `30 ~ 40`. Roughly, the game-tree complexity of `20^30 ~ 20^40 = 1e39 ~ 1e52` is gigantic indeed and it is comparable to Chess. (*Chess* is known as ~`1e47` and *Go* is `~1e170`)  

___Altough **Mariai** seems to be already beyond competent human players___, guessing next candidate moves well (*reduction of branching factor*) and estimating the current state or score better (*reduction of search depth*) are still challenging and a lot of room for improvement. So I decided to introduce neural networks here. I am not going to use the supervised training method because, as you know, there's no history or good game logs related to this game. Rather, now I am working on the reinforce training to do this. 

It would be the first BCF engine with no human-provided knowledge like AlphaGo-zero and Leela-zero.  
It also would be fun and great if we can compare moves from the ___BCF-zero___ with moves from ***Mariai***.



## Demo

Click to the images below to watch the games among the AIs and proficient players :-$

#### Sofiai (minimax algorithm) vs Mariai (MCTS, 1 million play-outs)

* 1st game (Mariai won)

  [![Sofiai vs Mariai 1](http://img.youtube.com/vi/tuo8HCQDbj4/0.jpg)](http://www.youtube.com/watch?v=tuo8HCQDbj4 "Sofiai vs Mariai 1")

* 2nd game (Mariai won)

  [![Sofiai vs Mariai 2](http://img.youtube.com/vi/mPzdNa05VTo/0.jpg)](http://www.youtube.com/watch?v=mPzdNa05VTo "Sofiai vs Mariai 2")


* 3rd game (Mariai won)

  [![Sofiai vs Mariai 3](http://img.youtube.com/vi/7rlDKj1kIUs/0.jpg)](http://www.youtube.com/watch?v=7rlDKj1kIUs "Sofiai vs Mariai 3")


* 4th game (Mariai won)

  [![Sofiai vs Mariai 4](http://img.youtube.com/vi/vzSlYbsdNe8/0.jpg)](http://www.youtube.com/watch?v=vzSlYbsdNe8 "Sofiai vs Mariai 4")


* 5th game (Maria won)

  [![Sofiai vs Mariai 5](http://img.youtube.com/vi/igPFkaastvs/0.jpg)](http://www.youtube.com/watch?v=igPFkaastvs "Sofiai vs Mariai 5")



#### Human(Francis Lim, Go Amateur 5d) vs Mariai (MCTS, 1 million play-outs)

* 1st game (Mariai won)

  [![Human vs Mariai 1](http://img.youtube.com/vi/1FLLGBP5lM8/0.jpg)](http://www.youtube.com/watch?v=1FLLGBP5lM8 "Human vs Mariai 1")

* 2nd game (Mariai won)

  [![Human vs Mariai 2](http://img.youtube.com/vi/LcDbIHQizXc/0.jpg)](http://www.youtube.com/watch?v=LcDbIHQizXc "Human vs Mariai 2")

* 3rd game (Mariai won)

  [![Human vs Mariai 3](http://img.youtube.com/vi/yKt-qEQpkW0/0.jpg)](http://www.youtube.com/watch?v=yKt-qEQpkW0 "Human vs Mariai 3")