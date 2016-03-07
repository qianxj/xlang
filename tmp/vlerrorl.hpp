//error text
WPP5Error( 0x10000001,	1,	err_text,					cn,		"%s")

//error for scan
WPP5Error( 0x10000010,	1,	err_token,					cn,		"error token")
WPP5Error( 0x10000011,	1,	err_doublescale,			cn,		"error read double scale")
WPP5Error( 0x10000012,	1,	err_scannumber,				cn,		"error scan number")
WPP5Error( 0x10000013,	1,	err_string,					cn,		"error string")

//token
WPP5Error( 0x10000020,	1,	err_needToken,				cn,		"需要'%s'")
WPP5Error( 0x10000021,	1,	err_needoperator,			cn,		"需要操作符 %s ")
WPP5Error( 0x10000022,	1,	err_needkn,					cn,		"需要换行符")
WPP5Error( 0x10000023,	1,	err_needsemicolon,			cn,		"需要 ';'")
WPP5Error( 0x10000024,	1,	err_needrightbracket,		cn,		"需要 ']'")
WPP5Error( 0x10000025,	1,	err_needrightbrace,			cn,		"需要 '}'")
WPP5Error( 0x10000026,	1,	err_needleftbracket,		cn,		"需要 '['")
WPP5Error( 0x10000027,	1,	err_needleftbrace,			cn,		"需要 '{'")
WPP5Error( 0x10000028,	1,	err_needleftparen,			cn,		"需要 '('")
WPP5Error( 0x10000029,	1,	err_needrightparen,			cn,		"需要 ')'")
WPP5Error( 0x1000002A,	1,	err_needcomma,				cn,		"需要 ','")
WPP5Error( 0x1000002B,	1,	err_needgt,					cn,		"需要 '>'")
WPP5Error( 0x1000002C,	1,	err_needcolon,				cn,		"需要 ':'")

//mistoken
WPP5Error( 0x10000030,	1,	err_needvarname,			cn,		"需要变量名")
WPP5Error( 0x10000031,	1,	err_needname,				cn,		"需要名称")
WPP5Error( 0x10000032,	1,	err_needident,				cn,		"需要标志符")
WPP5Error( 0x10000033,	1,	err_needcommenttrail,		cn,		"需要 */")
WPP5Error( 0x10000034,	1,	err_stringtrail,			cn,		"需要字符串结束符")
WPP5Error( 0x10000035,	1,	err_needtype,				cn,		"需要类型名")
WPP5Error( 0x10000036,	1,	err_needkeyword,			cn,		"需要关键字: %s")
WPP5Error( 0x10000037,	1,	err_needdeclarestmt,		cn,		"需要申明语句")
WPP5Error( 0x10000038,	1,	err_needstring,				cn,		"需要字符串")
//unknown
WPP5Error( 0x10000040,	1,	err_unvaliddataitem,		cn,		"无效的数据项")
WPP5Error( 0x10000041,	1,	err_unvalidcommand,			cn,		"无效的命令 %s")
WPP5Error( 0x10000042,	1,	err_unknownconst,			cn,		"不认识的常数")
WPP5Error( 0x10000043,	1,	err_unknownstmt,			cn,		"不认识的句子")
WPP5Error( 0x10000044,	1,	err_unvalidoperator,		cn,		"无效的操作符")

//Excess
WPP5Error( 0x10000050,	1,	err_notneedkeyword,			cn,		"不需要关键字")
WPP5Error( 0x10000051,	1,	err_unnecessarytoken,		cn,		"多余的符号")

//check
//type error
WPP5Error( 0x10000060,	1,	err_undefinedtype,			cn,		"未定义类型: %s")
WPP5Error( 0x10000061,	1,	err_type_op,				cn,		"%s的操作类型错误")
WPP5Error( 0x10000062,	1,	err_method_arg,				cn,		"%s 函数调用参数错误")
WPP5Error( 0x10000063,	1,	err_needcategoryint,		cn,		"需要整形类别的数据")
WPP5Error( 0x10000064,	1,	err_needclassorinteface,	cn,		"需要类或接口")
WPP5Error( 0x10000065,	1,	err_unvalidtype,			cn,		"无效的类别")
WPP5Error( 0x10000066,	1,	err_needtypeindent,			cn,		"类型定义缺少名称")
WPP5Error( 0x10000067,	1,	err_needclassorstruct,		cn,		"需要类类型")
WPP5Error( 0x10000068,	1,	err_needmethod,				cn,		"需要函数")
WPP5Error( 0x10000069,	1,	err_usedforclassmethod,		cn,		"%s只能用于非静态的类函数")
WPP5Error( 0x1000006A,	1,	err_neednumbertype,			cn,		"需要数字类型")

//axis error
WPP5Error( 0x10000070,	1,	err_needclassornamespace,	cn,		"需要类或命名空间")
WPP5Error( 0x10000071,	1,	err_neednamespace,			cn,		"需要命名空间")
//not have defined for use or repeat define
WPP5Error( 0x10000072,	1,	err_undefinedvar,			cn,		"未定义标志符: %s")
WPP5Error( 0x10000073,	1,	err_undefinedlang,			cn,		"未定义语言: %s")
WPP5Error( 0x10000074,	1,	err_muchdefine,				cn,		"多余的定义")
WPP5Error( 0x10000075,	1,	err_unknownmethod,			cn,		"未定义函数%s")
WPP5Error( 0x10000076,	1,	err_undeftype,				cn,		"未定义类型 %s")
WPP5Error( 0x10000077,	1,	err_existfield,				cn,		" %s 字段已存在")
WPP5Error( 0x10000078,	1,	err_existvar,				cn,		" %s 变量已存在")
WPP5Error( 0x10000079,	1,	err_existtype,				cn,		" %s 类型已存在")
WPP5Error( 0x1000007A,	1,	err_existmethod,			cn,		" %s 函数已存在")
WPP5Error( 0x1000007B,	1,	err_hasdefinedfield,		cn,		" %s 已定义成类字段")
WPP5Error( 0x1000007C,	1,	err_hasdefinedarg,			cn,		" %s 已定义成参数")
WPP5Error( 0x1000007D,	1,	err_needfield,				cn,		"%s不是合适的类字段")
WPP5Error( 0x1000007E,	1,	err_askclassornamespace,	cn,		"%s不是类或命名空间")

//need left value
WPP5Error( 0x10000080,	1,	err_needleftval,			cn,		"只有左值变量才能%s操作")
WPP5Error( 0x10000081,	1,	err_needleftvalex,			cn,		"只有左值变量才能操作")

//arry
WPP5Error( 0x10000090,	1,	err_needarraytype,			cn,		"需要数组类型")
WPP5Error( 0x10000091,	1,	err_needconstintrank,		cn,		"需要常量整数")

//warning	type group list
WPP5Error( 0x10001010,	1,	err_needtypegroupindent,	cn,		"类型组定义缺少名称")
WPP5Error( 0x10001011,	1,	err_typegroup_needitem,		cn,		"需要类型组项")
WPP5Error( 0x10001012,	1,	err_typegroup_needitemval,	cn,		"需要类型组项的值")
WPP5Error( 0x10001013,	1,	err_typegroupval_ref,		cn,		"类型组项 \"ref\" 只能是 true|false ")
WPP5Error( 0x10001014,	1,	err_typegroupval_copy,		cn,		"类型组项: \"copy\" 只能是 true|false ")
WPP5Error( 0x10001015,	1,	err_typegroupval_array,		cn,		"类型组项: \"array\" 只能是 true|false ")
WPP5Error( 0x10001016,	1,	err_typegroupval_fctpure,	cn,		"类型组项 \"function.pure\" 只能是 true|false ")
WPP5Error( 0x10001017,	1,	warn_typegroup_unvaditem,	cn,		"无效的类型组项\"%s\"")

//voc
WPP5Error( 0x10001040,	1,	err_needvocinstace,			cn,		"需要词汇实例")
WPP5Error( 0x10001041,	1,	err_vocundefpred,			cn,		"词汇未定义%s谓词")
WPP5Error( 0x10001042,	1,	err_casterror,				cn,		"相关类型无法转换")

//process error
WPP5Error( 0x10001060,	1,	err_actionshift,			cn,		"shift error")
WPP5Error( 0x10001061,	1,	err_actionreduce,			cn,		"reduce conflict")
WPP5Error( 0x10001062,	1,	err_actionshiftreduce,		cn,		"shift reduce conflict")
WPP5Error( 0x10001063,	1,	err_actionunknown,			cn,		"action unknown error")

