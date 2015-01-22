<?php

// Settings

function rol_login_settings($form, &$form_state){
	// Close default URLs
	$form['global_settings'] = array(
		'#title' => t('Global settings'),
		'#type' => 'fieldset'
	);
	$form['global_settings']['rol_login_close_default_urls'] = array(
		'#title' => t('Close default Drupal urls for login and registration'),
		'#type' => 'checkbox',
		'#default_value' => variable_get('rol_login_close_default_urls', 0),
		'#return_value' => 1,
	);

	// Logout
	$form['global_settings']['rol_login_logout_path'] = array(
		'#title' => t('Custom logout path').' (Drupal: user/logout)',
		'#type' => 'textfield',
		'#default_value' => variable_get('rol_login_logout_path', 'logout'),
		'#states' => array(
			'invisible' => array(
				':input[name="rol_login_close_default_urls"]' => array(
					'checked' => FALSE
				)
			)
		)
	);

	// Password recovery
	$form['global_settings']['rol_login_password_path'] = array(
		'#title' => t('Custom password recovery form path').' (Drupal: user/password)',
		'#type' => 'textfield',
		'#default_value' => variable_get('rol_login_password_path', 'password'),
		'#states' => array(
			'invisible' => array(
				':input[name="rol_login_close_default_urls"]' => array(
					'checked' => FALSE
				)
			)
		)
	);

	// Actions
	$form['global_settings']['actions']['#type'] = 'actions';
	$form['global_settings']['actions']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save configuration'),
		'#attributes' => array('style' => 'margin-bottom: 0;'),
		'#submit' => array('system_settings_form_submit')
	);

	return $form;
}

// Roles

function rol_login_admin_roles_list(){
	module_load_include('inc', 'rol_login');
	$list = _rol_login_list(true);

	$header = array(
		t('Login'),
		t('Registration'),
		t('Profile'),
		t('Roles'),
		t('Actions')
	);
	$body = array();
	$admin_url = variable_get('rol_login_admin_url', 'admin/config/people/rol-login').'/roles';

	foreach($list as $item){
		$row = array();

		$actions = l(t('Edit'), $admin_url.'/'.$item->rfid).' '.l(t('Delete'), $admin_url.'/'.$item->rfid.'/delete');

		$row[] = $item->login;
		$row[] = $item->registration;
		$row[] = $item->profile;
		$row[] = theme('item_list', array('items' => $item->roles));
		$row[] = $actions;

		$body[] = $row;
	}

	if(empty($body)){
		$body = array(
			'data' => array(
				array(
					'data' => t('Empty list'),
					'colspan' => 5
				)
			)
		);
	}

	return theme('table', array('header' => $header, 'rows' => $body));
}

function rol_login_admin_roles_add_form($form, &$form_state){
	module_load_include('inc', 'rol_login');

	$form['roles'] = array(
		'#title' => t('Roles'),
		'#type' => 'checkboxes',
		'#options' => _rol_login_get_allowed_roles(),
		'#required' => false,
	);

	$form['login'] = array(
		'#type' => 'textfield',
		'#title' => t('Login page'),
		'#description' => t('URL for login page.'),
		'#default_value' => '',
		'#required' => false,
	);

	$form['registration'] = array(
		'#type' => 'textfield',
		'#title' => t('Registration page'),
		'#description' => t('URL for registration page. Usefull only if registration is enabled for the defined rol.'),
		'#default_value' => '',
		'#required' => false,
	);

	$form['profile'] = array(
		'#type' => 'textfield',
		'#title' => t('Profile page'),
		'#description' => t('URL for profile page.'),
		'#default_value' => '',
		'#required' => false,
	);

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save settings'),
	);

	return $form;
}
function rol_login_admin_roles_add_form_submit($form, $form_state){
	module_load_include('inc', 'rol_login');

	$values = $form_state['values'];

	$rf = new stdClass;

	$rf->login = $values['login'];
	$rf->registration = $values['registration'];
	$rf->profile = $values['profile'];

	$rf->roles = array();
	foreach($values['roles'] as $rid => $active){
		if($active){
			$rf->roles[$rid] = $rid;
		}
	}

	_rol_login_add($rf);

	drupal_set_message(t('Login settings saved'));
	$form_state['redirect'] = variable_get('rol_login_admin_url', 'admin/config/people/rol-login').'/roles';
}

function rol_login_admin_roles_edit_form($form, &$form_state, $rf){
	$form['rf'] = array(
		'#type' => 'value',
		'#value' => $rf
	);

	$form['roles'] = array(
		'#title' => t('Roles'),
		'#type' => 'checkboxes',
		'#options' => user_roles(true),
		'#required' => true,
		'#default_value' => array_keys($rf->roles)
	);

	$form['login'] = array(
		'#type' => 'textfield',
		'#title' => t('Login page'),
		'#description' => t('URL for login page.'),
		'#default_value' => $rf->login,
		'#required' => false,
	);

	$form['registration'] = array(
		'#type' => 'textfield',
		'#title' => t('Registration page'),
		'#description' => t('URL for registration page. Usefull only if registration is enabled for the defined rol.'),
		'#default_value' => $rf->registration,
		'#required' => false,
	);

	$form['profile'] = array(
		'#type' => 'textfield',
		'#title' => t('Profile page'),
		'#description' => t('URL for profile page.'),
		'#default_value' => $rf->profile,
		'#required' => false,
	);

	$form['submit'] = array(
	'#type' => 'submit',
		'#value' => t('Save settings'),
	);

	return $form;
}
function rol_login_admin_roles_edit_form_submit($form, &$form_state){
	module_load_include('inc', 'rol_login');

	$values = $form_state['values'];
	$rf = $values['rf'];

	$rf->login = isset($values['login']) ? trim($values['login']) : '';
	$rf->registration = isset($values['registration']) ? trim($values['registration']) : '';
	$rf->profile = isset($values['profile']) ? trim($values['profile']) : '';

	$rf->roles = array();
	foreach($values['roles'] as $rid => $active){
		if($active){
			$rf->roles[$rid] = $rid;
		}
	}

	_rol_login_update($rf);

	$admin_url = variable_get('rol_login_admin_url', 'admin/config/people/rol-login').'/roles';
	$form_state['redirect'] = $admin_url;

	drupal_set_message(t('Login settings saved'));
}

function rol_login_admin_roles_delete_form($form, &$form_state, $rf){
	$form['rf'] = array(
		'#type' => 'value',
		'#value' => $rf
	);
	$form['login'] = array(
		'#type' => 'item',
		'#title' => t('Login'),
		'#markup' => $rf->login,
	);
	$form['registration'] = array(
		'#type' => 'item',
		'#title' => t('Registration'),
		'#markup' => $rf->registration,
	);
	$form['profile'] = array(
		'#type' => 'item',
		'#title' => t('Profile'),
		'#markup' => $rf->profile,
	);
	$form['roles'] = array(
		'#type' => 'item',
		'#title' => t('Roles'),
		'#markup' => theme('item_list', array('items' => $rf->roles))
	);

	return confirm_form($form, t('Are you sure you want to delete this form?'), variable_get('rol_login_admin_url', 'admin/config/people/rol-login').'/roles');
}
function rol_login_admin_roles_delete_form_submit($form, &$form_state){
	module_load_include('inc', 'rol_login');

	$rf = $form_state['values']['rf'];
	_rol_login_delete($rf);

	drupal_set_message(t('Settings deleted'));
	$form_state['redirect'] = variable_get('rol_login_admin_url', 'admin/config/people/rol-login').'/roles';
}

// User creation

function rol_login_admin_creation_list(){
	module_load_include('inc', 'rol_login');
	$list = _rol_login_creation_list(true);

	$header = array(
		t('Url'),
		t('Roles'),
		t('Actions')
	);
	$body = array();
	$admin_url = variable_get('rol_login_admin_url', 'admin/config/people/rol-login').'/user-creation';

	foreach($list as $item){
		$row = array();

		$actions = l(t('Edit'), $admin_url.'/'.$item->rcid).' '.l(t('Delete'), $admin_url.'/'.$item->rcid.'/delete');

		$row[] = $item->url;
		$row[] = theme('item_list', array('items' => $item->roles));
		$row[] = $actions;

		$body[] = $row;
	}

	if(empty($body)){
		$body = array(
			'data' => array(
				array(
					'data' => t('Empty list'),
					'colspan' => 3
				)
			)
		);
	}

	return theme('table', array('header' => $header, 'rows' => $body));
}

function rol_login_admin_creation_add_form($form, &$form_state){
	module_load_include('inc', 'rol_login');

	$form['roles'] = array(
		'#title' => t('Roles'),
		'#type' => 'checkboxes',
		'#options' => user_roles(true),
		'#required' => true,
	);

	$form['url'] = array(
		'#type' => 'textfield',
		'#title' => t('Url'),
		'#default_value' => '',
		'#required' => true,
	);

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save settings'),
	);

	return $form;
}
function rol_login_admin_creation_add_form_submit($form, &$form_state){
	module_load_include('inc', 'rol_login');

	$values = $form_state['values'];
	$rc = new stdClass;

	$rc->url = $values['url'];

	$rc->roles = array();
	foreach($values['roles'] as $rid => $active){
		if($active){
			$rc->roles[$rid] = $rid;
		}
	}

	_rol_login_creation_add($rc);

	drupal_set_message(t('Login settings saved'));
	$form_state['redirect'] = variable_get('rol_login_admin_url', 'admin/config/people/rol-login').'/user-creation';
}

function rol_login_admin_creation_edit_form($form, &$form_state, $rc){
	$form['rc'] = array(
		'#type' => 'value',
		'#value' => $rc
	);

	$form['roles'] = array(
		'#title' => t('Roles'),
		'#type' => 'checkboxes',
		'#options' => user_roles(true),
		'#required' => true,
		'#default_value' => array_keys($rc->roles)
	);

	$form['url'] = array(
		'#type' => 'textfield',
		'#title' => t('Url'),
		'#default_value' => $rc->url,
		'#required' => false,
	);

	$form['submit'] = array(
	'#type' => 'submit',
		'#value' => t('Save settings'),
	);

	return $form;
}
function rol_login_admin_creation_edit_form_submit($form, &$form_state){
	module_load_include('inc', 'rol_login');

	$values = $form_state['values'];
	$rc = $values['rc'];

	$rc->url = isset($values['url']) ? trim($values['url']) : '';
	$rc->roles = array();
	foreach($values['roles'] as $rid => $active){
		if($active){
			$rc->roles[$rid] = $rid;
		}
	}

	_rol_login_creation_update($rc);

	$admin_url = variable_get('rol_login_admin_url', 'admin/config/people/rol-login').'/user-creation';
	$form_state['redirect'] = $admin_url;

	drupal_set_message(t('Login settings saved'));
}

function rol_login_admin_creation_delete_form($form, &$form_state, $rc){
	$form['rc'] = array(
		'#type' => 'value',
		'#value' => $rc
	);
	$form['url'] = array(
		'#type' => 'item',
		'#title' => t('Login'),
		'#markup' => $rc->url,
	);
	$form['roles'] = array(
		'#type' => 'item',
		'#title' => t('Roles'),
		'#markup' => theme('item_list', array('items' => $rc->roles))
	);

	return confirm_form($form, t('Are you sure you want to delete this form?'), variable_get('rol_login_admin_url', 'admin/config/people/rol-login').'/user-creation');
}
function rol_login_admin_creation_delete_form_submit($form, &$form_state){
	module_load_include('inc', 'rol_login');

	$rc = $form_state['values']['rc'];
	_rol_login_creation_delete($rc);

	drupal_set_message(t('Settings deleted'));
	$form_state['redirect'] = variable_get('rol_login_admin_url', 'admin/config/people/rol-login').'/user-creation';
}
