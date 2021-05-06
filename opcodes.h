OPDEF(OPCODE_EXCHANGE, 4 , "~", "~", "")

OPDEF(OPCODE_LE   , 5 , "<=", "<=", "")
OPDEF(OPCODE_GE   , 6 , ">=", ">=", "")
OPDEF(OPCODE_EQ   , 7 , "==", "==", "")
OPDEF(OPCODE_NEQ  , 8 , "!=", "!=", "")
OPDEF(OPCODE_OR   , 10, "||", "||", "")
OPDEF(OPCODE_AND  , 11, "&&", "&&", "")

OPDEF(OPCODE_IF       , 12, "?" , "?" , "")
OPDEF(OPCODE_WHILE    , 13, ">|", ">|", "")
OPDEF(OPCODE_FOR      , 14, ">>", ">>", "")
OPDEF(OPCODE_BREAK    , 15, "|<", "|<", "")
OPDEF(OPCODE_CONTINUE , 16, "<<", "<<", "")
OPDEF(OPCODE_FOR_INFO , 17, "", "", "")
OPDEF(OPCODE_RET      , 18, "ret", "ret", "ret")

OPDEF(OPCODE_VAR_DEF  , 20, "", "", "")
OPDEF(OPCODE_ARR_DEF  , 21, "", "", "")
OPDEF(OPCODE_ARR_INFO , 22, "", "", "")
OPDEF(OPCODE_ARR_ASGN , 23, "", "", "")

OPDEF(OPCODE_ASGN_ADD , 24, "+=", "+=", "")
OPDEF(OPCODE_ASGN_SUB , 25, "-=", "-=", "")
OPDEF(OPCODE_ASGN_MUL , 26, "*=", "*=", "")
OPDEF(OPCODE_ASGN_DIV , 27, "/=", "/=", "")
OPDEF(OPCODE_ASGN_POW , 28, "^=", "^=", "")

OPDEF(OPCODE_IF_CONDITION   , 30, "", "", "")
OPDEF(OPCODE_COND_DEPENDENT , 31, "", "", "")

OPDEF(OPCODE_WHILE_CONDITION, 34, "", "", "")
OPDEF(OPCODE_WHILE_BODY     , 35, "", "", "")

OPDEF(OPCODE_ELEM_PUTN   , 100, "__PUT_NUMBER__", "__PUT_NUMBER__", "")
OPDEF(OPCODE_ELEM_PUTC   , 101, "__PUT_CHAR__"  , "__PUT_CHAR__"   , ""  )
OPDEF(OPCODE_ELEM_INPUT  , 102, "@", "@", "")
OPDEF(OPCODE_ELEM_MALLOC , 103, "#", "#", "")
OPDEF(OPCODE_ELEM_RANDOM , 104, "%", "%", "")
OPDEF(OPCODE_ELEM_EXIT   , 105, "exit", "exit ", "exit;")

OPDEF(OPCODE_ELEM_G_INIT      , 110, "__G_INIT__", "__G_INIT__", "")
OPDEF(OPCODE_ELEM_G_DRAW_ON   , 111, "", "", "")
OPDEF(OPCODE_ELEM_G_DRAW_OFF  , 112, "", "", "")
OPDEF(OPCODE_ELEM_G_DRAW_TICK , 113, "__G_TICK__", "__G_TICK__", "")
OPDEF(OPCODE_ELEM_G_FILL      , 114, "__G_FILL__", "__G_FILL__", "")
OPDEF(OPCODE_ELEM_G_PUT_PIXEL , 115, "__PUT_PIXEL__", "__PUT_PIXEL__", "")

OPDEF(OPCODE_VAR         , 200, "var", "var ", "")
OPDEF(OPCODE_FUNC        , 201, "func", "func ", "")
OPDEF(OPCODE_EXPR        , 202, "", "", "")
OPDEF(OPCODE_DEFAULT_ARG , 203, "", "", "")
OPDEF(OPCODE_CONTEXT_ARG , 204, "", "", "")

OPDEF(OPCODE_FUNC_INFO     , 300, "", "", "")
OPDEF(OPCODE_FUNC_DECL     , 301, "", "", "")
OPDEF(OPCODE_FUNC_CALL     , 302, "", "", "")
OPDEF(OPCODE_FUNC_ARG_DECL , 305, "", "", "")
OPDEF(OPCODE_FUNC_ARG_CALL , 306, "", "", "")
