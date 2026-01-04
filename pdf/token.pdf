#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "../include/token.h"
#include "../include/SinglyLinkedList.h"
#include "../include/operations.h"

token new_token(token_type type, char* lexeme){
    token tk;
    tk.type = type;
    
    if(lexeme != NULL){
        tk.lexeme = malloc(100);
        strcpy(tk.lexeme, lexeme);
    }
    else{
        tk.lexeme = NULL;
    }

    return tk;
}

void append_token(token_stream *tk_stream, token tok){
    if(tk_stream->count >= tk_stream->capacity){
        tk_stream->capacity *= 2;
        tk_stream->data = realloc(tk_stream->data, sizeof(token) * tk_stream->capacity);
        if(!tk_stream->data){
            fprintf(stderr, "realloc failed");
            exit(EXIT_FAILURE);
        }
        
    }
    tk_stream->data[tk_stream->count] = tok;
    tk_stream->count++;
}
token_stream *new_token_stream(){
    token_stream *tk_stream = malloc(sizeof(*tk_stream));

    tk_stream->capacity = 16;
    tk_stream->data = malloc(sizeof(token) * tk_stream->capacity);
    tk_stream->count = 0;
    tk_stream->index = 0;
    tk_stream->error = false;

    return tk_stream;
}

int peek_next_token_idx(size_t idx, token_stream *tk_stream){
    while(idx < tk_stream->count){
        if(tk_stream->data[idx].type == TOK_WS){
            idx++;
            continue;
        }
        else{
            return idx;
        }
    }
    return -1;
}

void free_token_stream(token_stream *tk_stream){
    for(size_t i = 0; i < tk_stream->count; i++){
        free(tk_stream->data[i].lexeme);
    }
    free(tk_stream->data);
    free(tk_stream);

}

char buff_peek_next(char *buff, size_t idx){
    return buff[idx + 1];
}

char *num_token_helper(char *num_lexeme){
    size_t len = strlen(num_lexeme) + strlen("Number()") + 1;
    char *buf = malloc(len);
    snprintf(buf, len, "Number(%s)", num_lexeme);
    return buf;
}

char *symbol_token_helper(char *sym_lexeme){
    size_t len = strlen(sym_lexeme) + strlen("Symbol(\"\")") + 1;
    char *buf = malloc(len);
    snprintf(buf, len, "Symbol(\"%s\")", sym_lexeme);
    return buf;
}

void null_term_helper(char *string, int len){ 
    string[len - 1] = '\0';
}

void print_tokens(token_stream *tk_stream){
    for(size_t i = 0; i < tk_stream->count; i++){
        printf("%s ", tk_stream->data[i].lexeme);
    }
}

void trim_new_line(token_stream *tk_stream){
    if(tk_stream->data[tk_stream->count - 1].type == TOK_WS){
        free(tk_stream->data[tk_stream->count - 1].lexeme);
        tk_stream->count--;
    }
}


token_stream *tokenize(FILE *fp){
    token_stream *tk_stream = new_token_stream();

    char stream_buf[4096];
    fgets(stream_buf, 4096, fp);
    printf("%s\n", stream_buf);

    int stream_idx = 0;
    char c = stream_buf[stream_idx];
    

    while(c != '\0'){
        switch(c){
            case ' ':// WS CHECK
                while (isspace(stream_buf[stream_idx]))
                {
                    stream_idx++;
                }
                append_token(tk_stream, new_token(TOK_WS, "WS"));
                break;
            case '\n': // WS CHECK - NEW LINE
                while (c == '\n')
                {
                    c = stream_buf[++stream_idx];
                }
                append_token(tk_stream, new_token(TOK_WS, "WS"));
                break;
            case '\t': // WS CHECK - T
                while (isspace(stream_buf[stream_idx]))
                {
                    stream_idx++;
                }
                append_token(tk_stream, new_token(TOK_WS, "WS"));
                break;
            case ';':// COMMENT
                char tmp_buff[100];
                int idx = 0;
                c = stream_buf[++stream_idx]; // skip ;
                while(c != '\0' && c != '\n'){
                    tmp_buff[idx] = c;
                    idx++;
                    c = stream_buf[++stream_idx];
                }
                tmp_buff[idx] = '\0';
                append_token(tk_stream, new_token(TOK_COMMENT, tmp_buff));
                break;
            case '#':// BOOL
                if(buff_peek_next(stream_buf, stream_idx) == 't'){
                    append_token(tk_stream, new_token(TOK_BOOL, "Bool(true)"));
                }
                else if (buff_peek_next(stream_buf, stream_idx) == 'f'){
                    append_token(tk_stream, new_token(TOK_BOOL, "Bool(false)"));
                }
                else{
                    char invalid[3] = {c, buff_peek_next(stream_buf, stream_idx), '\0'};
                    append_token(tk_stream, new_token(TOK_INVALID, invalid));
                    stream_idx = stream_idx + 2;
                    break;
                }
                stream_idx = stream_idx + 2;
                c = stream_buf[stream_idx];
                break;
            case '\'':// SHORT HAND QUOTE
                append_token(tk_stream, new_token(TOK_SYM, "quote"));
                c = stream_buf[++stream_idx];
                break;
            case '(':// OPEN PAREN
                append_token(tk_stream, new_token(TOK_LIST_L, "("));
                c = stream_buf[++stream_idx];
                break;
            case ')':// CLOSE PAREN
                append_token(tk_stream, new_token(TOK_LIST_R, ")"));
                c = stream_buf[++stream_idx];
                break;
            default:
                if((c == '+' || c == '-') && isdigit(buff_peek_next(stream_buf, stream_idx))){ //NUM CHECK WITH +/-

                    
                    char tmp_buff[100];
                    tmp_buff[0] = c;
                    c = stream_buf[++stream_idx];
                    int idx = 1;
                    while(isdigit(c)){
                        tmp_buff[idx] = c;
                        idx++;
                        c = stream_buf[++stream_idx];
                    }
                    tmp_buff[idx] = '\0';

                    
                    append_token(tk_stream, new_token(TOK_NUM, tmp_buff));
                    break;
                }
                else{
                    char tmp_buff[100];
                    int idx = 0;
                    bool is_num = true;

                    while (!isspace(c) && c != ')' && c != '(' && c != '\0') {
                        tmp_buff[idx] = c;
                        idx++;
                        if (!isdigit(c)) {
                            is_num = false;
                        }
                        c = stream_buf[++stream_idx];
                        
                    }
                    tmp_buff[idx] = '\0';
                    char lead_chk[2] = {tmp_buff[0], '\0'};

                    if (is_num) {  // NUM CHECK NORMAL UNSIGNED
                        append_token(tk_stream, new_token(TOK_NUM, tmp_buff));
                        break;
                    } else {  // SYMBOL CHECK
                        if(symbol_check(tmp_buff) != -1){
                            
                            append_token(tk_stream, new_token(TOK_SYM, tmp_buff));
                            break;
                        }
                        if (isdigit(tmp_buff[0]) || (symbol_check(lead_chk) != -1 && strlen(tmp_buff) > 1)) {
                                
                            append_token(tk_stream, new_token(TOK_INVALID, tmp_buff));
                            
                            break;
                        }
                        // if((symbol_check(lead_chk)) == -1 && strlen(tmp_buff)
                        // > 1) // COMMON SYMBOL - NOT QUIT
                        // {
                        //     append_token(tk_stream, new_token(TOK_INVALID,
                        //     tmp_buff)); break;
                        // }

                        append_token(tk_stream, new_token(TOK_SYM, tmp_buff));
                        break;
                    }
                }
        }
        c = stream_buf[stream_idx];
    }
    trim_new_line(tk_stream);
    return tk_stream;
}