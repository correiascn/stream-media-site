<html>

<head>
<title>StreamViewer</title>
</head>

<body>

<?php
	$username =  $_POST['username'];
	$stream = $_POST['stream'];
	$post_text = $_POST['post_text'];

	echo "<hr>";
	echo "<div style = 'font:33px/54px Arial'> StreamViewer - Post Results</div>";
	echo "Current user: $username <br>";

	$cmd = "./post $stream $username -n $post_text";
	$output = array();
	exec($cmd, $output, $status);
	foreach($output as $line) echo $line . '<br>';

	/* Output page elements. */
	$cmd = "./wpmlp post_result.wpml";
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
