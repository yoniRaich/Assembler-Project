#include "assembler.h"

int second_pass(FILE *exp_file_handler, char *file_name, plw head_IC,
                plw head_DC, TypeLabel *symbols_table)
{
  int line_number = 1;
  int is_entry = FALSE;
  int error = FALSE;
  int g_error = FALSE;
  int label_base_val = 0;
  int label_offset_val = 0;
  int externs_file_created = FALSE;
  int entries_file_created = FALSE;
  ARE label_are;
  char line[MAX_LINE];
  char token[MAX_LINE];
  FILE *obj_file_handler;
  FILE *ent_file_handler;
  FILE *ext_file_handler;

  /*returns if failed to create an output file*/
  if (load_file(&obj_file_handler, file_name, ".ob", "w"))
    return 1;

  while (fgets(line, MAX_LINE, exp_file_handler))
  {
    get_first_token(line, token);
    if (!strcmp(token, ".entry"))
    {
      get_next_token(line, token);
      error = process_entry_label(token, line_number, symbols_table, &is_entry);
    }
    else if (token[strlen(token) - 1] == ':')
      get_next_token(line, token);
    else if (!strcmp(token, ".extern"))
    {
      get_next_token(line, token);
      get_next_token(line, token);
    }

    if (!is_empty_line(line) && !error && found_label(line, token, symbols_table))
    {
      error = get_label_values(token, &label_base_val, &label_offset_val,
                               &label_are, line_number, symbols_table);
      if (!error && label_are == E)
      {
        if (!externs_file_created)
        {
          if (load_file(&ext_file_handler, file_name, ".ext", "w"))
            return 1;
          externs_file_created = TRUE;
        }
        fprintf(ext_file_handler, "%s BASE %d\n", token,
                set_next_empty(head_IC, label_are, label_base_val));
        fprintf(ext_file_handler, "%s OFFSET %d\n\n", token,
                set_next_empty(head_IC, label_are, label_offset_val));
      }
      else if (!error)
      {
        if (is_entry)
        {
          if (!entries_file_created)
          {
            if (load_file(&ent_file_handler, file_name, ".ent", "w"))
              return 1;
            entries_file_created = TRUE;
          }
          fprintf(ent_file_handler, "%s, %d, %d\n", token, label_base_val,
                  label_offset_val);
          get_next_token(line, token);
          if (!is_empty_line(line))
          {
            error = TRUE;
            fprintf(stderr,
                    "Error at line %d: Extended text after entry variable\n",
                    line_number);
          }
          is_entry = FALSE;
        }
        else
        {
          set_next_empty(head_IC, label_are, label_base_val);
          set_next_empty(head_IC, label_are, label_offset_val);
        }
      }
    }

    if (error)
    {
      g_error = TRUE;
      error = FALSE;
    }
    line_number++;
  }

  fprintf(obj_file_handler, "\t\t\t%d\t%d\n", get_length(head_IC),
          get_length(head_DC));
  load_obj_file(head_IC, obj_file_handler);
  load_obj_file(head_DC, obj_file_handler);
  fclose(obj_file_handler);
  if (externs_file_created)
    fclose(ext_file_handler);
  if (entries_file_created)
    fclose(ent_file_handler);
  return g_error;
}

int process_entry_label(char *token, int line_number, TypeLabel *symbols_table, int *is_entry)
{
  int error = FALSE;
  if (is_label_exists(token, symbols_table))
  {
    if (!is_defined_entry(token, symbols_table))
    {
      add_entry_attribute(token, symbols_table);
      *is_entry = TRUE;
    }
  }
  else
  {
    error = TRUE;
    fprintf(stderr, "Error at line %d: '%s' is an illegal label name\n",
            line_number, token);
  }
  return error;
}
