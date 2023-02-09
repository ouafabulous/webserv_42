#pragma once
#ifndef CONFIG_HPP
# define CONFIG_HPP

#define EPOLL_BACKLOG	4096
#define CONN_TO_LISTEN	64
#define BUFFER_SIZE		4096
#define CH_PERM			0644
#define READ 0
#define WRITE 1

// ERRORS
#define INIT_ERROR_HEADER	"[INIT ERROR]:\t"
#define INIT_SUCCESS_HEADER	"[INIT SUCCESS]:\t"

#define ROUTINE_ERROR_HEADER	"[RUN ERROR]:\t"

#endif
