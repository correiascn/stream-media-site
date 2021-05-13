<html>

<head>
<title>StreamViewer</title>
</head>

<body>

<hr>

<?php

	/* Index = log in page */
	/* Only the title is not outputted via wpmlp. It is typically paired with echoing a variable, which can not be done in pure HTML, and thus, the config file. */
	echo "<div style = 'font:33px/54px Arial'> StreamViewer - Login</div>";

	/* Output page elements. */
	$cmd = "./wpmlp index.wpml";
	exec($cmd, $output, $status);
	foreach($output as $line) echo $line;
?>

</body>
</html>