<?php
include("root.php");
include($ROOT."common.php");
preambule("Publications - Abstracts", "publi");
?>
<p>

<table>

<tr valign="top">
<td align="right" class="bibtexnumber">
[<a name="vee/10/geoffray/vmkit">1</a>]
</td>
<td class="bibtexitem">
Nicolas Geoffray, Ga&euml;l Thomas, Julia Lawall, Gilles Muller, and Bertil
  Folliot.
 VMKit: a substrate for managed runtime environments.
 In <em>Proceedings of the international conference on Virtual
  Execution Environments, VEE&nbsp;'10</em>, pages 51-62, Pittsburgh, PA, USA, 2010.
  ACM.
[&nbsp;<a href="Publications_bib.php#vee/10/geoffray/vmkit">bib</a>&nbsp;| 
<a href="geoffray10vee-vmkit.pdf">.pdf</a>&nbsp;]
<blockquote><font size="-1">

Managed Runtime Environments (MREs), such as the JVM and the CLI, form
an attractive environment for program execution, by providing
portability and safety, via the use of a bytecode language and
automatic memory management, as well as good performance, via
just-in-time (JIT) compilation. Nevertheless, developing a fully
featured MRE, including e.g. a garbage collector and JIT compiler, is
a herculean task. As a result, new languages cannot easily take
advantage of the benefits of MREs, and it is difficult to experiment
with extensions of existing MRE based languages.<p>
This paper describes and evaluates VMKit, a first attempt to build a
common substrate that eases the development of high-level MREs. We
have successfully used VMKit to build two MREs: a Java Virtual Machine
and a Common Language Runtime. We provide an extensive study of the
lessons learned in developing this infrastructure, and assess the ease
of implementing new MREs or MRE extensions and the resulting
performance. In particular, it took one of the authors only one month
to develop a Common Language Runtime using VMKit. VMKit furthermore
has performance comparable to the well established open source MREs
Cacao, Apache Harmony and Mono, and is 1.2 to 3 times slower than
JikesRVM on most of the DaCapo benchmarks.
  
</font></blockquote>
<p>
</td>
</tr>


<tr valign="top">
<td align="right" class="bibtexnumber">
[<a name="dsn/09/geoffray/ijvm">2</a>]
</td>
<td class="bibtexitem">
Nicolas Geoffray, Ga&euml;l Thomas, Gilles Muller, Pierre Parrend, St&eacute;phane
  Fr&eacute;not, and Bertil Folliot.
 I-JVM: a Java virtual machine for component isolation in OSGi.
 In <em>Proceedings of the international conference on Dependable
  Systems and Networks, DSN&nbsp;'09</em>, pages 544-553, Estoril, Portugal, 2009. IEEE
  Computer Society.
[&nbsp;<a href="Publications_bib.php#dsn/09/geoffray/ijvm">bib</a>&nbsp;| 
<a href="geoffray09dsn-ijvm.pdf">.pdf</a>&nbsp;]
<blockquote><font size="-1">

The OSGi framework is a Java-based, centralized, component oriented
platform. It is being widely adopted as an execution environment for
the development of extensible applications. However, current Java
Virtual Machines are unable to isolate components from each other. For
instance, a malicious component can freeze the complete platform by
allocating too much memory or alter the behavior of other components
by modifying shared variables.  This paper presents I-JVM, a Java
Virtual Machine that provides a lightweight approach to isolation
while preserving compatibility with legacy OSGi applications. Our
evaluation of I-JVM shows that it solves the 8 known OSGi
vulnerabilities that are due to the Java Virtual Machine and that the
overhead of I-JVM compared to the JVM on which it is based is below
20%.
  
</font></blockquote>
<p>
</td>
</tr>


<tr valign="top">
<td align="right" class="bibtexnumber">
[<a name="pppj/08/geoffray/ladyvm">3</a>]
</td>
<td class="bibtexitem">
Nicolas Geoffray, Ga&euml;l Thomas, Charles Cl&eacute;ment, and Bertil Folliot.
 A lazy developer approach: building a JVM with third party
  software.
 In <em>Proceedings of the international symposium on Principles and
  Practice of Programming in Java, PPPJ&nbsp;'08</em>, pages 73-82, Modena, Italy,
  2008. ACM.
[&nbsp;<a href="Publications_bib.php#pppj/08/geoffray/ladyvm">bib</a>&nbsp;| 
<a href="geoffray08pppj-ladyvm.pdf">.pdf</a>&nbsp;]
<blockquote><font size="-1">

The development of a complete Java Virtual Machine (JVM)
implementation is a tedious process which involves knowledge in 
different areas: garbage collection, just in time compilation, 
interpretation, file parsing, data structures, etc.
The result is that developing its own virtual machine requires a 
considerable amount of man/year. In this paper
we show that one can implement a JVM with third party
software and with performance comparable to industrial and
top open-source JVMs. Our proof-of-concept implementation 
uses existing versions of a garbage collector, a just in
time compiler, and the base library, and is robust enough to
execute complex Java applications such as the OSGi Felix
implementation and the Tomcat servlet container.  

</font></blockquote>
<p>
</td>
</tr>
</table>
<?php epilogue() ?>
