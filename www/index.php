<?php

$domain=ereg_replace('[^\.]*\.(.*)$','\1',$_SERVER['HTTP_HOST']);
$group_name=ereg_replace('([^\.]*)\..*$','\1',$_SERVER['HTTP_HOST']);

echo '<?xml version="1.0" encoding="UTF-8"?>';
?>
<!DOCTYPE html
	PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
	"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en   ">

  <head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
	<title><?php echo $project_name; ?></title>
	<script language="JavaScript" type="text/javascript">
	<!--
	function help_window(helpurl) {
		HelpWin = window.open( helpurl,'HelpWindow','scrollbars=yes,resizable=yes,toolbar=no,height=400,width=400');
	}
	// -->
		</script>

<style type="text/css">
	<!--
	BODY {
		margin-top: 3;
		margin-left: 3;
		margin-right: 3;
		margin-bottom: 3;
		background: #5651a1;
	}
	ol,ul,p,body,td,tr,th,form { font-family: verdana,arial,helvetica,sans-serif; font-size:small;
		color: #333333; }

	h1 { font-size: x-large; font-family: verdana,arial,helvetica,sans-serif; }
	h2 { font-size: large; font-family: verdana,arial,helvetica,sans-serif; }
	h3 { font-size: medium; font-family: verdana,arial,helvetica,sans-serif; }
	h4 { font-size: small; font-family: verdana,arial,helvetica,sans-serif; }
	h5 { font-size: x-small; font-family: verdana,arial,helvetica,sans-serif; }
	h6 { font-size: xx-small; font-family: verdana,arial,helvetica,sans-serif; }

	pre,tt { font-family: courier,sans-serif }

	a:link { text-decoration:none }
	a:visited { text-decoration:none }
	a:active { text-decoration:none }
	a:hover { text-decoration:underline; color:red }

	.titlebar { color: black; text-decoration: none; font-weight: bold; }
	a.tablink { color: black; text-decoration: none; font-weight: bold; font-size: x-small; }
	a.tablink:visited { color: black; text-decoration: none; font-weight: bold; font-size: x-small; }
	a.tablink:hover { text-decoration: none; color: black; font-weight: bold; font-size: x-small; }
	a.tabsellink { color: black; text-decoration: none; font-weight: bold; font-size: x-small; }
	a.tabsellink:visited { color: black; text-decoration: none; font-weight: bold; font-size: x-small; }
	a.tabsellink:hover { text-decoration: none; color: black; font-weight: bold; font-size: x-small; }
	-->
</style>

</head>

<body>

<table border="0" width="100%" cellspacing="0" cellpadding="0">

	<tr>
		<td><a href="/"><img src="http://<?php echo $domain; ?>/themes/inria/images/logo.png" border="0" alt="" width="198" height="52" /></a></td>
	</tr>

</table>

<table border="0" width="100%" cellspacing="0" cellpadding="0">

	<tr>
		<td>&nbsp;</td>
		<td colspan="3">


		<!-- start tabs -->

	<tr>
		<td align="left" bgcolor="#E0E0E0" width="9"><img src="http://<?php echo $domain; ?>/themes/inria/images/tabs/topleft.png" height="9" width="9" alt="" /></td>
		<td bgcolor="#E0E0E0" width="30"><img src="http://<?php echo $domain; ?>/themes/inria/images/clear.png" width="30" height="1" alt="" /></td>
		<td bgcolor="#E0E0E0"><img src="http://<?php echo $domain; ?>/themes/inria/images/clear.png" width="1" height="1" alt="" /></td>
		<td bgcolor="#E0E0E0" width="30"><img src="http://<?php echo $domain; ?>/themes/inria/images/clear.png" width="30" height="1" alt="" /></td>
		<td align="right" bgcolor="#E0E0E0" width="9"><img src="http://<?php echo $domain; ?>/themes/inria/images/tabs/topright.png" height="9" width="9" alt="" /></td>
	</tr>

	<tr>

		<!-- Outer body row -->

		<td bgcolor="#E0E0E0"><img src="http://<?php echo $domain; ?>/themes/inria/images/clear.png" width="10" height="1" alt="" /></td>
		<td valign="top" width="99%" bgcolor="#E0E0E0" colspan="3">

			<!-- Inner Tabs / Shell -->

			<table border="0" width="100%" cellspacing="0" cellpadding="0">
			<tr>
				<td align="left" bgcolor="#ffffff" width="9"><img src="http://<?php echo $domain; ?>/themes/inria/images/tabs/topleft-inner.png" height="9" width="9" alt="" /></td>
				<td bgcolor="#ffffff"><img src="http://<?php echo $domain; ?>/themes/inria/images/clear.png" width="1" height="1" alt="" /></td>
				<td align="right" bgcolor="#ffffff" width="9"><img src="http://<?php echo $domain; ?>/themes/inria/images/tabs/topright-inner.png" height="9" width="9" alt="" /></td>
			</tr>

			<tr>
				<td bgcolor="#ffffff"><img src="http://<?php echo $domain; ?>/themes/inria/images/clear.png" width="10" height="1" alt="" /></td>
				<td valign="top" width="99%" bgcolor="white">

	<!-- whole page table -->
<table width="100%" cellpadding="5" cellspacing="0" border="0">
<tr><td width="65%" valign="top">

WARNING: Private Projects' webspace is not private. It is visible for everybody with an internet connection.<br>
ATTENTION : l'espace web des projets privés n'est pas privé. Il est visible par tous les utilisateurs d'Internet. 
</td>

<td width="35%" valign="top">

		<p>&nbsp;</p>
</td></tr></table>
			&nbsp;<p>
                        <center>
                               In case of problems, mail the <a href="mailto:help.et.gforge[at]inria.fr">administrators</a> or file a <a href="http://gforge.inria.fr/tracker/?func=browse&group_id=1&atid=110">bug</a>
                        </center>
			<!-- end main body row -->


				</td>
				<td width="10" bgcolor="#ffffff"><img src="http://<?php echo $domain; ?>/themes/inria/images/clear.png" width="2" height="1" alt="" /></td>
			</tr>
			<tr>
				<td align="left" bgcolor="#E0E0E0" width="9"><img src="http://<?php echo $domain; ?>/themes/inria/images/tabs/bottomleft-inner.png" height="11" width="11" alt="" /></td>
				<td bgcolor="#ffffff"><img src="http://<?php echo $domain; ?>/themes/inria/images/clear.png" width="1" height="1" alt="" /></td>
				<td align="right" bgcolor="#E0E0E0" width="9"><img src="http://<?php echo $domain; ?>/themes/inria/images/tabs/bottomright-inner.png" height="11" width="11" alt="" /></td>
			</tr>
			</table>

		<!-- end inner body row -->

		</td>
		<td width="10" bgcolor="#E0E0E0"><img src="http://<?php echo $domain; ?>/themes/inria/images/clear.png" width="2" height="1" alt="" /></td>
	</tr>
	<tr>
		<td align="left" bgcolor="#E0E0E0" width="9"><img src="http://<?php echo $domain; ?>/themes/inria/images/tabs/bottomleft.png" height="9" width="9" alt="" /></td>
		<td bgcolor="#E0E0E0" colspan="3"><img src="http://<?php echo $domain; ?>/themes/inria/images/clear.png" width="1" height="1" alt="" /></td>
		<td align="right" bgcolor="#E0E0E0" width="9"><img src="http://<?php echo $domain; ?>/themes/inria/images/tabs/bottomright.png" height="9" width="9" alt="" /></td>
	</tr>
</table>

<!-- PLEASE LEAVE "Powered By Gforge" on your site -->
<br />
<center>
<a href="http://gforge.org/"><img src="http://<?php echo $domain; ?>/images/pow-gforge.png" alt="Powered By GForge Collaborative Development Environment" border="0" /></a>
</center>


</body>
</html>
