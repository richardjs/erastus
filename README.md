# Erastus: Santorini AI

Erastus is an AI engine for playing the board game
[Santorini](https://en.wikipedia.org/wiki/Santorini_(game)). You can play
against it online at https://erastus-ai.fly.dev. You may want to learn [the
rules](https://roxley.com/santorini-rulebook) first (Erastus only plays the
basic game).

Erastus uses a [Monte Carlo tree
search](https://en.wikipedia.org/wiki/Monte_Carlo_tree_search) algorithm that
gets stronger as it runs more iterations. The Fly instance linked above is
capped at 40K iterations, but experimentation shows that playing strength
improves up to around 200K iterations. Parallel searches can also be ran
simultaneously across different workers to further increase strength. The Fly
instance is capped to 1 worker, but experimentation suggests that strength
improves up to at least 8 workers.

An easy way to run your own instance is to use [Docker
Compose](https://docs.docker.com/compose/) and the provided `docker-compose.yml`
file. The app will be accessible at http://127.0.0.1:9000.

Erastus was created in 2022 by [Richard Schneider](https://schneiderbox.net).
