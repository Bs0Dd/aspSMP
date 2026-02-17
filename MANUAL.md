# Инструкция по сборке и использованию aspSMP

## Сборка

Для начала необходимо купить/добыть/найти сам USBasp (а точнее два, если у вас еще нет ни одного).
Их вид может отличаться, но все они имеют 10-пиновый разъем.
Стоимость модуля - около 4$ за штуку. Настоятельно советую взять их вместе со шнурком (как на фото):

<p align="center">
  <img width="400px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/usbasp.jpg" alt="USBasp" />
</p>

Берем шнурок и отрезаем один конец - место отрезания выбираем на свое усмотрение.

<p align="center">
  <img width="400px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/asm0.jpg" alt="Assembly_0" />
</p>

Для работы нам нужны только следующие провода. Отделяем их и зачищаем.

<p align="center">
  <img width="400px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/asm1.jpg" alt="Assembly_1" />
</p>

Теперь необходимо изготовить разъем для СМП модуля. В идеале, лучше всего купить заводские соединители
(ОНп-КС-10-4/10x2,5-В21 счетверенный и ОНп-КС-10-2/5x2,5-В21 сдвоенный) и склеить их, но найти столь
экзотический продукт довольно трудно.

<p align="center">
  <img width="400px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/onpks.png" alt="ONp-KS" />
</p>

Поэтому существует практически ничем не уступающее решение на основе современных и дешевых компонентов.  

Варианта сборки два, начнем с наиболее легкого:

### Легкий способ

Для этого можно закупить комплект дешевых макетных проводов Папа-Папа с разъемами Dupont/GoldPin.
Как правило сами штырьки здесь полые и легко деформируются - этим и воспользуемся.

<p align="center">
  <img width="400px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/novi1.jpg" alt="Novice_1" />
  <img width="400px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/novi2.jpg" alt="Novice_2" />
</p>

Плоскогубцами и хорошим усилием необходимо расплющить штыри, сделав их плоскими и широкими.

<p align="center">
  <img width="400px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/novi3.jpg" alt="Novice_3" />
  <img width="400px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/novi4.jpg" alt="Novice_4" />
</p>

Чтобы в последствии штыри не болтались в СМП и имели хороший контакт - необходимо с одной стороны (удобнее
с внутренней) покрыть их небольшим слоем свинцового припоя с помощью паяльника. Таким образом толщина штыря
станет ближе к таковой в оригинальных соединителях. После этого стоит проверить, хорошо ли получившиеся
штыри заходят в отверстия разъема СМП и не болтаются ли там.

<p align="center">
  <img width="400px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/novi5.jpg" alt="Novice_5" />
</p>

Теперь остается лишь повернуть их под 90° и склеить между собой цианоакрилатным клеем (супер-клей).
Опционально добавить метку, чтобы не перепутать стороны при вставке СМП.

<p align="center">
  <img width="400px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/novi6.jpg" alt="Novice_6" />
</p>

Вторую половину проводов отрезать и припаять к подготовленным проводам на шнурке USBasp.
Схема такова:

<p align="center">
  <img src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/smp_adapter.gif" alt="SMP Adapter" />
  <img width="600px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/novi7.jpg" alt="Novice_7" />
</p>

Переходник USBasp-СМП готов.

<p align="center">
  <img width="600px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/novi8.jpg" alt="Novice_8" />
</p>


### Вариант для продвинутых

Можно сделать и более цивилизованный вариант шнура, если имеется опыт обжимки соединителей Dupont.
Для этого потребуется 5 пинов Dupont (Папа), 6 или 5-местный разъем и, разумеется, заготовленный шлейф от USBasp.
Единственная ремарка, что конец шлейфа в таком случае будет удобнее зачистить не сразу, а делать это с каждым
проводом по отдельности по мере обжимки, в таком случае жилы соседних проводов не будут мешаться.

<p align="center">
  <img width="400px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/profi1.jpg" alt="Professional_1" />
</p>

Пять выделенных проводов необходимо обжать пинами. Обратите внимание, что при обжиме, если пины находятся внутренней
стороной наружу, то помеченный (первый) провод долен быть слева. В таком случае лицевая сторона разъема будет
соответствовать лицевой стороне модуля СМП.

<p align="center">
  <img width="400px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/profi2.jpg" alt="Professional_2" />
</p>

Надеваем разъем. Важно учесть, что при использовании 6-местного разъема первый его пин (со стрелкой) должен
остаться пустым. В таком случае нумерация контактов будет соотвествовать таковой в СМП.

<p align="center">
  <img width="400px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/profi3.jpg" alt="Professional_3" />
</p>

Теперь, по аналогии с первым способом, необходимо расплющить пины плоскогубцами (или иным инструментом).

<p align="center">
  <img width="400px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/profi4.jpg" alt="Professional_4" />
</p>

А затем и покрыть оловом с внутренней стороны.

<p align="center">
  <img width="400px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/profi5.jpg" alt="Professional_5" />
</p>

И после этого, с помощью пинцета повернуть внешнюю часть пина на 90 для соответствия ориентации пинов в СМП.

<p align="center">
  <img width="400px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/profi6.jpg" alt="Professional_6" />
</p>

Если получившийся соединитель плохо заходит в СМП или, наоборот, сидит в нем недостаточно надежно -
стоит уменьшить или увеличить слой олова на пинах. Переходник USBasp-СМП готов.

<p align="center">
  <img width="600px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/profi7.jpg" alt="Professional_"7 />
</p>


## Прошивка

Джампер выбора напряжения на USBasp устанавливаем в 5V, далее менять его не потребуется.
Замыкаем расположенные рядом с ним (или в указанном производителем месте) штыри обновления прошивки
программатора.

<p align="center">
  <img width="400px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/jumps.jpg" alt="Jumpers" />
</p>

Берем второй USBasp, где также выставляем напряжение в 5V (штыри обновления не замыкаем) и шнурком от него
соединям оба программатора.

<p align="center">
  <img width="400px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/twoasp.jpg" alt="Two programmers" />
</p>

Подключаем второй программатор к компьютеру - на обоих USBasp должны загореться светодиоды LED1.

<p align="center">
  <img width="400px" src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/conn.jpg" alt="Connected" />
</p>

Если у вас Linux - драйвера, как правило, уже встроены в систему. Достаточно убедиться в наличии устройста
командой `lsusb`.

<p align="center">
  <img src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/lsusb.png" alt="lsusb" />
</p>

Для Windows необходимо установить драйвер. Воспользуемся программой Zadig (включена в [комплект](https://github.com/Bs0Dd/aspSMP/releases)).
После ее открытия в списке появится устройство `USBasp`. Выберите и установите для него драйвер
`libusb-win32`.

<p align="center">
  <img src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/zadig.png" alt="Zadig" />
</p>

Если с драйвером возникнет проблема - попробуйте сменить его на `libusbK`.

<p align="center">
  <img src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/oops.png" alt="Driver problem" />
</p>

Теперь необходима программа `avrdude`. Версия для Windows прилагается к комплекту. Пользователям
Linux необходимо установить ее самостоятельно (например `sudo apt install avrdude` для Debian/Ubuntu).
Далее для прошивки можно воспользоваться либо скриптом flash(.bat/.sh), либо вручную выполнив в консоли
команду типа `avrdude -c usbasp -p m8 -U flash:w:aspsmp.hex`.

<p align="center">
  <img src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/avrdude.png" alt="AVRDUDE" />
</p>

Если прошивка завершилась успешно - можно отсоединить программатор от ПК и отключить от него прошитый USBasp.
С прошитого программатора снимается перемычка обновления прошивки.


## Работа

Теперь в получившийся программатор aspSMP можно вставить шнур-переходник и подключить к нему модуль СМП.
Подключаем устройство к компьютеру. Ранее установленные Zadig драйвера автоматически применятся и для aspSMP.

<p align="center">
  <img src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/asmpdev.png" alt="aspSMP device" />
</p>

Дальнейшая работа производится с помощью программы `aspsmp`.

<p align="center">
  <img src="https://raw.githubusercontent.com/Bs0Dd/aspSMP/master/graphics/asptool.png" alt="aspSMP tool" />
</p>

aspSMP умеет выполнять с модулями СМП три действия: read, write, init.

* Read (чтение) - позволяет считать полный образ памяти модуля в файл.
  Например: `aspsmp read -f mpo10.bin`
* Write (запись) - позволяет записать содержимое любого файла в память модуля.
  Например: `aspsmp write -f dino.bin`
* Init (инициализация) - проводит инициализацию памяти модуля с помощью команды стирания.
  Например: `aspsmp init`

По умолчанию инициализация модуля (заполнение) производится байтом 0x20 (32), однако его можно изменить.  
Например: `aspsmp init -i 42` - заполнит СМП значением 0x2A (42).

Инициализация модуля необходима на случай если, например, при ошибочном подключениии СМП к программатору
(в таком случае поведение непредсказуемо) контроллер СМП испортит данные в ОЗУ и перейдет в режим "замка",
перестав реагировать на команды чтения (считывается 0xFF) и записи (ошибка записи).
В этом случае модуль можно разблокировать либо передачей ему команды разблокировки с кодовым словом
(первые 8 байт в ОЗУ), на что дается три попытки. Либо проинциализировать память СМП, заполнив его, начиная
с адреса 0xFFFF (обязательное требование, несмотря на меньший фактический размер), произвольным байтом
с помощью команды стирания.  
Аналогичный эффект можно получить с помощью команды `INIT` в БЕЙСИКе МК90.

Если при чтении СМП образ памяти состоит из нулей (хотя СМП не пуст), вероятно, между СМП и aspSMP нет
удовлетворительного контакта. Стоит проверить изготовленный шнур.

На этом все!

---

Составление инструкции:  
2026 © Bs0Dd [[bs0dd.net]](http://bs0dd.net)
