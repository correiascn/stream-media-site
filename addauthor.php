<html>

<head>
<title>StreamViewer</title>
</head>

<body>

<?php
	$username =  $_POST['username'];
	/* Only the title is not outputted via wpmlp. It is typically paired with echoing a variable, which can not be done in pure HTML, and thus, the config file. */
	echo "<hr>";
	echo "<div style = 'font:33px/54px Arial'> StreamViewer - Add Author</div>";

	echo "Current user: $username";

	/* Output page elements. */
	$cmd = "./wpmlp addauthor.wpml";
	exec($cmd, $output, $status);
	foreach($output as $line)
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
