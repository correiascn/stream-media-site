<html>

<head>
<title>StreamViewer</title>
</head>

<body>

<?php
	$username =  $_POST['username'];

	/* All code not doable in HTML or not specified in the WPML file format. */
	echo "<hr>";
	echo "<div style = 'font:33px/54px Arial'> StreamViewer - Make a Post</div>";
	echo "Current user: $username";

	/* Output page elements. */
	$cmd = "./wpmlp post.wpml";
	exec($cmd, $output, $status);
	foreach($output as $line)
	{
		/* Note: all forms read from the config file use POST for forms. */
		if ($line == "</form>" || $line == '\t<input type = "Submit" value = "Submit">')
		{
			/* Store all hidden variables whenever a form is encountered. */
			echo "<input type='hidden' name='username' value= '" . $username . "'>";
		}
		echo $line;
	}
?>

</body>
</html>
