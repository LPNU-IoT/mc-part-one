
# 🛠️ Мікроконтролери частина 1 (Інструкція для студентів)

## 📚 Про предмет

**Мікроконтролери частина 1** — це курс, на якому студенти вивчають основи роботи з мікроконтролерами (MCU), їх переваги, особливості та подальші напрямки розвитку в Embedded Systems, System Engineering та інших сферах. Курс забезпечує базові знання для просування в галузі.

## 📁 Структура репозиторія

Цей репозиторій має наступну структуру:  
├── `REPOSITORY_OF_COURSE`  
&emsp;&emsp; ├── **.github/workflows** — папка з GitHub Actions, що автоматично перевіряє ваш код. 🚀  
&emsp;&emsp; ├── **labs** — папка з лабораторними роботами, в якій містяться всі необхідні. 🔬  
&emsp;&emsp; ├── **lectures** — папка з лекціями. 📘  
&emsp;&emsp; └── **README.md** — дана інструкція. 📄

> **🚨 УВАГА!**  
> Папку `.github/workflows` категорично заборонено змінювати.  
> Будь-які зміни в цій папці можуть призвести до автоматичного зниження балу за лабораторну роботу. ⚠️

## 🛠️ Як працювати з репозиторієм

1. **Створення гілки**: Для виконання кожної нової лабораторної роботи створіть окрему гілку від основної гілки `main` або `master` за наступним принципом:
   ```bash
   git clone ${this_repository}
   git branch --list
   >>> main (active branch)
   git checkout -b ${student_surname_name}/labX  
     
   `X` — номер лабораторної роботи.  
   `student_surname_name` — Прізвища та ім'я студента  
   ``` 

  
2. **Робота з лабораторними роботами**: Помістіть вашу лабораторну роботу в відповідну папку у вашій гілці, яка буде називатися `labX`, де `X` знову номер лабораторної.

3. **Пул-реквест**: Щоб відправити вашу лабораторну роботу на перевірку, створіть пул-реквест на основну гілку (`main` або `master`) та чекайте, поки GitHub Actions виконає перевірку. 🔄

## ✅ Критерії перевірки

Ваш код буде перевірятися за наступними критеріями:

1. **Компільованість**: Код має успішно компілюватися. 💻
2. **Стиль коду**: Код має відповідати заданому стилю написання. ✍️
3. **Чистота коду**: Код не має містити зайвого сміття, яке не використовується. 🧹

> Якщо під час перевірки буде виявлено помилки за цими критеріями, вам потрібно їх виправити і знову запушити зміни у вашу гілку. Лабораторні роботи, які не пройшли перевірку, не будуть прийняті до перевірки. ❌

## 📝 Додаткові зауваження

- Будь ласка, уважно дотримуйтеся всіх інструкцій і не змінюйте конфігурацію GitHub Actions.
- Переконайтеся, що ви уважно прочитали всі інструкції в лабораторних роботах та дотримуєтеся їх.
- Всі лабораторні роботи повинні бути виконані та подані в межах термінів, визначених у плані курсу. ⏳

## ❓ Часті питання (FAQ)

- **Що робити, якщо я отримав помилку у перевірці?**  
  Виправте помилки відповідно до повідомлень в GitHub Actions і знову запуште зміни. 🔧

- **Як створити пул-реквест?**  
  Ви можете знайти детальні інструкції у [документації GitHub](https://docs.github.com/en/pull-requests). 📚

- **Куди звертатися за допомогою?**  
  Якщо у вас є питання або проблеми, звертайтеся до викладача або на форум курсу. 🗣️

### Бажаємо успіхів у навчанні! 🚀


> Introduction to Embedded Systems: Using ANSI C and the Arduino Development Environment. 
> 
> Slack: https://app.slack.com/huddle/T277SDD26/C9P6S41DH
>
> Proteus IoT lib:
> https://github.com/LPNU-IoT/proteus-wemosd1-lib
> 
> **Recomended material:**
> 1. https://www.jimfiore.org/Books.html
> 2. https://docs.arduino.cc/
> 3. https://www.arduino.cc/reference/en/
> 4. https://docs.arduino.cc/hardware/mega-2560/
> 5. https://arduino.github.io/arduino-cli/0.35/
> 
> Simulation example:
> https://wokwi.com/projects/391005481372432385
