HTTP/1.1 200 OK
Content-type: text/html

<?php
$cookieValue = '';

if (isset($_COOKIE['myCookie'])) {
    $cookieValue = $_COOKIE['myCookie'];
}

if (isset($_POST['myVariable'])) {
    // Set a cookie with the value of the POST variable
    setcookie('myCookie', $_POST['myVariable'], time() + 3600); // Expires in 1 hour
    $cookieValue = $_POST['myVariable'];
}
?>

<!DOCTYPE html>
<html>
	<head>
	</head>
	<body>
    <form method="POST" action="">
        <label for="myVariable">Enter a value:</label>
        <input type="text" name="myVariable" id="myVariable" value="<?php echo htmlspecialchars($cookieValue); ?>">
        <button type="submit">Submit</button>
    </form>
</body>
</html>
