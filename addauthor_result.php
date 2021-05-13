<html>

<head>
<title>StreamViewer</title>
</head>

<body>

<?php
	
	$username =  $_POST['username'];
	$list = $_POST['stream_list'];

	echo "<hr>";
	echo "<div style = 'font:33px/54px Arial'> StreamViewer - Add Author Results</div>";

	echo "Current user: $username <br><br>";

	$progcmd = "./addauthor $list $username";
	$output = array();
	exec($progcmd, $output, $status);
	
	/* Print out addauthor output. */
	foreach($output as $line) echo $line . "<br>";

	/* Now output page elements. */
	$cmd = "./wpmlp addauthor_result.wpml";
	$output2 = array();
	exec($cmd, $output2, $status);

	foreach($output2 as $line)
	{
		/* Note: all forms read from the config file use POST for forms. */
		if ($line == "</form>")
		{
			/* Store all hidden variables whenever a form is encountered. */
			echo "<input type='hidden' name='username' value= '" . $username . "'>";
		}
		echo $line;
	}
?>

</body>
</html>
