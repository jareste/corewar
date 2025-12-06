/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jareste- <jareste-@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 12:24:26 by jareste-          #+#    #+#             */
/*   Updated: 2023/05/08 23:47:53 by jareste-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_INTERNAL_H
# define PARSE_INTERNAL_H

/* parse_util.c */
void	m_print_instr_args(t_instr *inst);
void	m_print_instr_noop(t_instr *inst);
void	m_print_instrs(t_instr *inst, t_label *label);
int		m_is_pc_relative_op(const t_op *op);
char	*m_skip_spaces(char *s);

/* parse_utils2.c */
t_label	*m_new_label(const char *name);
t_op	*m_find_op(const char *name, size_t len);
int		m_parse_reg(const char *s, int *out);
int		m_parse_num32(const char *s, int32_t *out);
int		m_mask_for_arg_type(t_arg_type t);

/* parse_arg.c */
int		m_parse_arg_token(const char *arg_str, t_arg *out);

/* offsets.c */
int		m_find_nearest_offset(t_instr *inst_list, int line_no, int prog_size);
int		m_compute_inst_size(t_instr *inst);
int		m_compute_all_instructions_size(t_instr *inst_list);
void	m_check_orphan_labels(t_label *label_list, t_instr *inst_list);
void	m_compute_all_labels(t_label *label_list, t_instr *il, int off);
int		m_compute_offsets(t_instr *inst_list, t_label *label_list);

/* instr.c */
t_instr	*m_new_instruction(char *instr_text, int line_no);

#endif /* PARSE_INTERNAL_H */
