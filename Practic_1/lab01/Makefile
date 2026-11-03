build: example add_serial add_parallel exemplu_chat

example: example.c
	gcc example.c -o example -lpthread

add_serial: add_serial.c
	gcc add_serial.c -o add_serial

add_parallel: add_parallel.c
	gcc add_parallel.c -o add_parallel -lpthread

exemplu_chat: exemplu_chat.c
	gcc exemplu_chat.c -o exemplu_chat -lpthread

.PHONY: clean
clean:
	rm -f example add_serial add_parallel exemplu_chat
