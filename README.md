<p align="center"><img width="100" src="./show/clam.png"></p>

<div align="center">English | <a href="https://github.com/umatobu/clamlog/blob/master/README.zh_CN.md">简体中文</a></div>

<h2 align="center">clamlog - a colored log library for qt5</h2>



# Effect

![clamlog](./show/show1.png)
![clamlog](./show/show2.png)
![clamlog](./show/show3.png)

# Instructions

## qt creator

xxx.pro add:

```
win32 {
    contains(CONFIG, console) {
        DEFINES += ULOG_COLOR_CONSOLE_STYLE
    } else {
        DEFINES += ULOG_COLOR_UNIX_STYLE
    }
} else {
    DEFINES += ULOG_COLOR_UNIX_STYLE
}
```

## VS

if console, add macro:

```
ULOG_COLOR_CONSOLE_STYLE
```

else if no console, add macro:

```
ULOG_COLOR_MSVC_STYLE
```

# Expansion

Overload <<:

```
void operator << (doubel message)
{
    log(QString::number(nummber));
}
```

```
struct Human {
    Human(const QString &name_in, const int &age_in)
        : name(name_in)
        , age(age_in)
    {}
    QString name;
    int     age;
};

void operator << (Human h)
{
    log(QStringLiteral("Human(%1, '%2')").arg(h.name).arg(h.age););
}
```


