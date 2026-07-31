/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ereali <ereali@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 10:31:30 by ereali            #+#    #+#             */
/*   Updated: 2022/03/07 18:45:21 by ereali           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# define NAME "minishell"
# include <stddef.h>
# include <fcntl.h>
# include <limits.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdbool.h>
# include <errno.h>
# include <stdio.h>
# include <sys/types.h>
# include <dirent.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <signal.h>
# include "../srcs/libft/libft.h"
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "../srcs/containers/containers.h"
# include "parsing.h"
# include <readline/readline.h>
# include "../srcs/gnl/get_next_line.h"

# ifndef NSIG
#  define NSIG 64
# endif

typedef struct s_var_list
{
	char				*key;
	char				*value;
	bool				is_exported;
	struct s_var_list	*next;
}				t_var_list;

enum e_redir {
	INPUT,
	OUTPUT,
	CLOBBER,
	APPEND,
	HERE,
	DUPIN,
	DUPOUT,
	RW
};

typedef struct s_redir
{
	enum e_redir	type;
	char			*word;
	int				newfd;
	int				oldfd;
	int				fd_save;
	struct s_redir	*next;
}				t_redir;

typedef struct s_simple {
	t_redir			*redir_list;
	t_token_list	*argv_tokens;
	char			**argv;
}				t_simple;

union	u_command
{
	struct s_simple		simple;
	struct s_pipeline	*pipeline;
	struct s_list		*list;
	struct s_grouping	*grouping;
};

enum e_command_type
{
	SIMPLE,
	PIPELINE,
	LIST,
	GROUPING,
	PARSE_ERROR,
	PARSE_FATAL
};

typedef struct s_command
{
	enum e_command_type	type;
	union u_command		command;
}				t_command;

typedef struct s_pipeline
{
	struct s_pipeline	*next;
	struct s_command	command;
}			t_pipeline;

typedef struct s_grouping
{
	struct s_command	command;
	t_redir				*redir_list;
	bool				is_in_subshell;
}				t_grouping;

//utilisation de AND_IF OR_IF et SEMI

typedef struct s_list
{
	struct s_list		*next;
	struct s_command	command;
	t_token_type		sep;
}				t_list;

enum e_fd_status {
	FD_CLOSE,
	FD_OPEN,
	FD_TMP
};

typedef void			(*t_sighandler)(int);

typedef struct s_env
{
	t_var_list		*vars;
	t_char_vec		opened_files;
	t_sighandler	default_signals[64];
	bool			is_interactive;
	t_command		*current_tree_root;
	unsigned char	err;
}				t_env;

typedef enum e_status {OK, KO, FATAL}	t_status;

typedef unsigned char	(*t_builtin)(char **, t_env *);
typedef t_status		(*t_command_handler)(union u_command, t_env *env);
extern bool				g_int;

// env manip:
t_status		remove_var(t_env *env, char *key);
int				add_var(t_env *env, char *key, char *value, bool is_exported);
t_status		set_var(t_env *env, char *key, char *value, bool is_exported);
t_status		export_env(t_var_list *env, char *key);
char			*get_var_val(t_env *env, char *key);
t_var_list		*get_var_ptr(t_env *env, char *key);
void			free_env(t_env *env);
t_var_list		*dup_var_list(t_var_list *src) __attribute__((malloc));
// env conversion:
char			**serialize_env(t_var_list *env) __attribute__((malloc));
t_status		parse_env(char **envp, t_env *env);
//env initialisation:
t_status		init_env(t_env *env);
// path_utils:
bool			path_has_dot(char *path);
char			*ft_realpath(const char *path, char *resolved_path);

// utils
t_status		my_get_working_directory(const char *for_whom, char **dst);
void			my_getopt(char ***argv, char *option, char dest[256]);
t_status		set_signal(int sig, t_sighandler action, t_env *env);
void			reset_signals(t_env *env);
void			sigint_handler(int sig);
void			path_pop(char *path);
t_status		path_push(char *path, char *component);
t_status		my_perror(t_env *env, char *error[2], bool use_errno,
					t_status ret);
bool			is_dir(char *path);
t_status		my_tmp_file(int *fd, char **dst);
// int				isvalid_name_letter(char c);

//builtin
unsigned char	sh_echo(char **argv, t_env *env);
unsigned char	sh_pwd(char **argv, t_env *env);
unsigned char	sh_env(char **argv, t_env *env);
unsigned char	sh_export(char **argv, t_env *env);
unsigned char	sh_unset(char **argv, t_env *env);
unsigned char	sh_exit(char **argv, t_env *env);
unsigned char	sh_cd(char **argv, t_env *env);
// input
char			*my_readline(t_env *env, char *prompt) __attribute__((malloc));
t_status		readnline(char **line, t_env *env);
// exec
t_status		path_find(char *name, t_env *env, char **dst);
t_status		get_err(t_env *env, pid_t pid);
t_status		exec_command(t_command cmd, t_env *env);
t_status		redir(int oldfd, int newfd, int *save, t_env *env);
t_status		perform_redirection(t_env *env, t_redir *list);
void			clean_fds(t_env *env);
t_status		reset_redirection(t_env *env, t_redir *list);
t_status		perform_assignments(t_env *env, t_simple cmd, bool export);
t_builtin		is_special_built_in(char *name);
t_builtin		is_built_in(char *name);
t_status		exec_special_builtin(t_simple s, t_env *env);
t_status		exec_regular_builtin(t_simple s, t_env *env);
t_status		exec_program(char *name, t_simple s, t_env *env);
t_status		exec_pipeline(union u_command cmd, t_env *env);
t_status		my_fork(pid_t *dst, t_env *env);
void			ft_exit(t_env *env) __attribute__((noreturn));

//lst :
t_var_list		*ft_lstlast(t_var_list *lst);
size_t			ft_lstsize(t_token_list *lst);
t_token_list	*ft_lstcpy(t_token_list *current);
t_token_list	*ft_lstnew(t_token_type token);

//parsing :
t_status		add_token(t_token_list **node, size_t len, t_token_list **line,
					char *str);
t_status		tokenise(char *str, t_token_list **dst, t_env *env);
void			free_token_list(t_token_list *ptr);
t_status		parse_tree(t_token_list *current, t_command *tree, t_env *env);
void			ft_prin(t_token_list	**line);
void			ft_prin_redir(t_redir	**line);
void			free_redir(t_redir *list);
void			destroy_tree(t_command *c);
void			free_redir(t_redir *list);
void			ft_print_pipe(t_pipeline **line);
void			ft_print_list(t_list **line);
void			ft_print_grouping(t_grouping **line);
int				isvalid_name_letter(char c);
t_status		new_redir_list(t_token_list **current, t_redir **dst,
					t_env *env);
t_command		parse_grouping(t_token_list **current, t_env *env);
t_command		parse_pipe(t_token_list **current, t_command prev_command,
					t_env *env);
t_command		parse_list(t_token_list **current, t_command prev_command,
					t_env *env);
t_command		parse_simple(t_token_list **current, t_env *env);
t_command		parsing(t_token_list **current, t_token_type expected,
					t_env *env);
void			ft_lstadd_back_redir(t_redir **alst, t_redir *new);
void			destroy_grouping(union u_command c);
void			destroy_list(union u_command c);
void			destroy_pipeline(union u_command c);
void			destroy_simple(union u_command c);
t_command		parse_error(t_command to_destroy[2], t_command ret);
t_status		ft_lstinsertword(t_token_list **alst, char *str);
t_status		ft_heredoc(t_env *env, t_redir *redir);
t_status		remove_quotes(char *str);

//expansion
t_status		expand_path(t_token_list *lst, t_str_vec *dst);
t_status		ft_fillargv(t_simple *command);
t_status		ft_field_split(t_token_list **lst);
t_status		found_val(char **new, char *key, t_env *env);
char			*ft_replacekey(size_t i, size_t j, char *str, t_env *env);
char			*expand_word_all(char *str, t_env *env);
t_status		expand_simple(t_simple *command, t_env *env);
t_status		path_match(char *str, t_str_vec *dst);
t_status		expand_redir(t_redir **first, t_env *env);
t_status		path_match_current(char *glob, t_str_vec *dst);

#endif
