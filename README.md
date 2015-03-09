# hw1
<h1 id="задание-первое">Задание первое</h1>
<p>В первом задании необходимо реализовать примитивный вариант утилиты <code>cat</code>, копирующий символы из stdin на stdout.</p>
<h2 id="первая-часть">Первая часть</h2>
<p>Требуется реализовать динамическую библиотеку, содержащую функции-хелперы <code>read_</code> и <code>write_</code>, делающие то же, что и <code>read</code> и <code>write</code>, но для буфера целиком (либо до EOF). Сигнатуры хелперов должны совпадать с сигнатурами оригинальных функций.</p>
<h3 id="файлы-в-репозитории">Файлы в репозитории</h3>
<ul>
<li><code>/lib/helpers.h</code></li>
<li><code>/lib/helpers.c</code></li>
<li><code>/lib/Makefile</code></li>
</ul>
<h3 id="скриптом-сборки-генерируется">Скриптом сборки генерируется</h3>
<ul>
<li><code>/lib/libhelpers.so</code></li>
</ul>
<h3 id="hints">Hints</h3>
<ul>
<li><code>man 2 read</code></li>
<li><code>man 2 write</code></li>
</ul>
<h2 id="вторая-часть">Вторая часть</h2>
<p>Используя функции-хелперы из первой части, реализовать утилиту <code>cat</code>. В качестве аргумента <code>fd</code> хелперам <code>read_</code> и <code>write_</code> необходимо передавать <code>STDIN_FILENO</code> и <code>STDOUT_FILENO</code> соответственно.</p>
<h3 id="файлы-в-репозитории-1">Файлы в репозитории</h3>
<ul>
<li><code>/cat/cat.c</code></li>
<li><code>/cat/Makefile</code></li>
</ul>
<h3 id="скриптом-сборки-генерируется-1">Скриптом сборки генерируется</h3>
<ul>
<li><code>/cat/cat</code></li>
</ul>
<h3 id="пример-работы">Пример работы</h3>
<ul>
<li><code>./cat &lt; cat.c &gt; cat2.c &amp;&amp; diff cat.c cat2.c &amp;&amp; echo OK</code></li>
</ul>
<h2 id="дедлайн">Дедлайн</h2>
<ul>
<li><code>11 марта, 06:00 (GMT+3)</code></li>
</ul>
