<?php

// Closed user pages >> 404

function _user_closed_page(){
	drupal_set_breadcrumb(array());

	return MENU_NOT_FOUND;
}

// Custom user_view_page function

function _user_view_page($account){
	global $user;

	// Si es la propia página de usuario, se redirige al profile
	if($account && $account->uid && $user->uid == $account->uid){
		module_load_include('inc', 'rol_login');
		$rf = _rol_login_get_user_settings($user);
		drupal_goto($rf->profile);
	}else{
		return user_view($account);
	}
}

// Custom login page

function rol_login_login_page($rf){
	global $user;

	if($user && $user->uid){
		module_load_include('inc', 'rol_login');
		$rf = _rol_login_get_user_settings();

		// TODO conectar con login destination para definir la redirección dependiendo del rol del usuario
		$destination = variable_get('rol_login_close_default_urls', false) ? $rf->profile : 'user';
		drupal_goto($destination);
	}else{
		module_load_include('inc', 'rol_login');
		$form = drupal_get_form('user_login', $rf);

		return $form;
	}
}

// Custom profile pages

function rol_login_profile_page(){
	global $user;

	if($user && $user->uid){
		$account = user_load($user->uid);
		drupal_set_title('Mi cuenta - '.$user->name);

		return array(
			'user' => user_view($account)
		);
	}else{
		drupal_set_breadcrumb(array());
		return MENU_NOT_FOUND;
	}
}

function rol_login_profile_edit_page(){
	global $user;

	if($user && $user->uid){
		drupal_set_title('Editar cuenta - '.$user->name);

		return menu_execute_active_handler('user/'.$user->uid.'/edit', FALSE);
	}else{
		drupal_set_breadcrumb(array());
		return MENU_NOT_FOUND;
	}
}

// Custom register page

function rol_login_register_page($rf){
	global $user;

	if($user && $user->uid){
		module_load_include('inc', 'rol_login');
		$rf = _rol_login_get_user_settings();

		// TODO conectar con login destination para definir la redirección dependiendo del rol del usuario
		$destination = variable_get('rol_login_close_default_urls', false) ? $rf->profile : 'user';
		drupal_goto($destination);
	}else{
		return drupal_get_form('user_register_form', $rf);
	}
}

// User creation page
function rol_login_user_creation_page($rc){
	return drupal_get_form('rol_login_user_creation_form', $rc);
}

// Custom reset password callback

function rol_login_pass_reset($form, &$form_state, $uid, $timestamp, $hashed_pass, $action = false) {
	global $user;

	module_load_include('inc', 'rol_login');

	// When processing the one-time login link, we have to make sure that a user
	// isn't already logged in.
	if ($user->uid) {
		// The existing user is already logged in.
		if ($user->uid == $uid) {
			$rf = _rol_login_get_user_settings($user);
			$edit_profile_path = $rf->profile.'/edit';
			drupal_set_message(t('You are logged in as %user. <a href="!user_edit">Change your password.</a>', array('%user' => $user->name, '!user_edit' => url($edit_profile_path))));
		}
		// A different user is already logged in on the computer.
		else {
			$reset_link_account = user_load($uid);
			if (!empty($reset_link_account)) {
				drupal_set_message(t('Another user (%other_user) is already logged into the site on this computer, but you tried to use a one-time link for user %resetting_user. Please <a href="!logout">logout</a> and try using the link again.',
				array('%other_user' => $user->name, '%resetting_user' => $reset_link_account->name, '!logout' => url(variable_get('rol_login_logout_path', 'logout')))));
			} else {
				// Invalid one-time link specifies an unknown user.
				drupal_set_message(t('The one-time login link you clicked is invalid.'));
			}
		}
		drupal_goto();
	}
	else {
		// Time out, in seconds, until login URL expires. Defaults to 24 hours =
		// 86400 seconds.
		$timeout = variable_get('user_password_reset_timeout', 86400);
		$current = REQUEST_TIME;
		// Some redundant checks for extra security ?
		$users = user_load_multiple(array($uid), array('status' => '1'));
		if ($timestamp <= $current && $account = reset($users)) {
			// No time out for first time login.
			if ($account->login && $current - $timestamp > $timeout) {
				drupal_set_message(t('You have tried to use a one-time login link that has expired. Please request a new one using the form below.'));
				drupal_goto(variable_get('rol_login_password_path', 'password'));
			}
			elseif ($account->uid && $timestamp >= $account->login && $timestamp <= $current && $hashed_pass == user_pass_rehash($account->pass, $timestamp, $account->login, $account->uid)) {
				// First stage is a confirmation form, then login
				if ($action == 'login') {
					// Set the new user.
					$user = $account;
					// user_login_finalize() also updates the login timestamp of the
					// user, which invalidates further use of the one-time login link.
					user_login_finalize();
					watchdog('user', 'User %name used one-time login link at time %timestamp.', array('%name' => $account->name, '%timestamp' => $timestamp));
					drupal_set_message(t('You have just used your one-time login link. It is no longer necessary to use this link to log in. Please change your password.'));
					// Let the user's password be changed without the current password check.
					$token = drupal_random_key();
					$_SESSION['pass_reset_' . $user->uid] = $token;

					$rf = _rol_login_get_user_settings($user);
					$edit_profile_path = $rf->profile.'/edit';

					drupal_goto($edit_profile_path, array('query' => array('pass-reset-token' => $token)));
				}
				else {
					$form['message'] = array('#markup' => t('<p>This is a one-time login for %user_name and will expire on %expiration_date.</p><p>Click on this button to log in to the site and change your password.</p>', array('%user_name' => $account->name, '%expiration_date' => format_date($timestamp + $timeout))));
					$form['help'] = array('#markup' => '<p>' . t('This login can be used only once.') . '</p>');
					$form['actions'] = array('#type' => 'actions');
					$form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Log in'));
					$form['#action'] = url(variable_get('rol_login_password_path', 'password')."/reset/$uid/$timestamp/$hashed_pass/login");

					return $form;
				}
			}
			else {
				drupal_set_message(t('You have tried to use a one-time login link that has either been used or is no longer valid. Please request a new one using the form below.'));
				drupal_goto(variable_get('rol_login_password_path', 'password'));
			}
		}
		else {
			// Deny access, no more clues.
			// Everything will be in the watchdog's URL for the administrator to check.
			drupal_access_denied();
			drupal_exit();
		}
	}
}

function rol_login_password_recovery_info_page(){
	$roles = isset($_SESSION['password_recovery_info']) ? $_SESSION['password_recovery_info'] : array();

	return theme('password_recovery_info', array('roles' => $roles));
}

