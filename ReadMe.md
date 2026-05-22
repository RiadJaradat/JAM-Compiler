<h1>Jam Compiler</h1>
<p>
Compiler built on a <b>STRONG BASE</b>
where everything is open source
</p>

<h1>its foundation is based </h1>
<li> namespace <b>"mc"</b> which contains all machine code instructions that are needed to create a runnable ELF excutable on <b>linux</b>.</li>
<li> namespace <b>"helper"</b> which contains repetitive fucntions such as print and exit.</li>
<li> class <b>generall_jam_emmiter</b> or <b>Emmiter</b> which emmits the correct elf, program header, sections, and the binary code for the program which makes the program runnable without many code (Low level API).</li>
<hr style="width: 80%;">
<br>
<h1> Syntax </h1>
<p>
its syntax will be dynamic based on the program needed for the exact project its being used in.
<br>
<b>e.g:
<br>
* if the program is simple and does not use much of the compilx features: Jam`print("Hello, JAM!!")`
<br>
* if the program needs program arguments, it could start with Jam`int main(int argc, char *argv[]) { ... }`
<br>
* if the program needs OOP <br>
<div style="background-color: rgb(10, 10, 10); border-radius: 10px; text-allign: center;"> 
Jam <br>`class Foo( #inheritance ) {
    <br>public:
        <br>
        void __init(self, *args) {
            <br> always runs on init <br>
        } <br>
}`
</div>
</p>
<h1>Goal</h1>
<p>
the goal is to creat a language that is: <br>
    
    * low level
    * simple
    * fun to learn!

this will make the language both beginner friendly and complex 
<h1><b>All At Once!!</b></h1>
</p>

<p>
<h3>current state of the project:</h3>
the project right now is at where the backend generall emmiter which is speciallised for <b>JAM</b> is actively being devoloped.
<h3>Problems where the most chalanges are:</h3>

* syntax parser
* syntax hanling
* syntax in generall :)
<br>
<br>
<h1>Thank you!</h1>
<center><h4>* last update on <b>'Date May 22 2026'</b></h4></center>
<hr>
</p>