<?php

function account_fields_admin_page($form, &$form_state){
	$role_names = user_roles(true);
	$list_fields = field_info_instances('user', 'user');

	$form['account_fields'] = array('#tree' => true);
	$default = variable_get('account_fields', array());
	foreach($role_names as $rid => $role){
		foreach($list_fields as $field){
			if($field['deleted']){
				continue;
			}

			$label = $field['label'];
			$fid = $field['field_id'];
			$name = $field['field_name'];
			$required = isset($default[$rid]) && isset($default[$rid][$name]) ? $default[$rid][$name]['required'] : 0;
			$visible = isset($default[$rid]) && isset($default[$rid][$name]) ? $default[$rid][$name]['visible'] : 1;

			$form['account_fields'][$rid][$name]['visible'] = array(
				'#title' => t('Visible'),
				'#type' => 'checkbox',
				'#return_value' => 1,
				'#default_value' => $visible
			);
			$form['account_fields'][$rid][$name]['required'] = array(
				'#title' => t('Required'),
				'#type' => 'checkbox',
				'#return_value' => 1,
				'#default_value' => $required,
				'#states' => array(
					'disabled' => array(
						':input[name="account_fields['.$rid.']['.$name.'][visible]"]' => array(
							'checked' => false
						)
					),
				)
			);
		}
	}

	$form['actions'] = array(
		'#type' => 'actions'
	);
	$form['actions']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save configuration')
	);

	$form['#submit'][] = 'system_settings_form_submit';

	return $form;
}

function theme_account_fields_admin_page($variables) {
	$role_names = user_roles(true);
	$list_fields = field_info_instances('user', 'user');
	$form = $variables['form'];

	$header = array(t('Field'));
	foreach($role_names as $rid => $role){
		$header[] = $role_names[$rid];
	}

	$rows = array();
	foreach($list_fields as $field){
		$label = $field['label'];
		$fid = $field['field_id'];
		$name = $field['field_name'];

		$row = array();
		$row[] = array(
			'data' => $name.' [<em>'.$label.'</em>]',
			'class' => array('field')
		);
		foreach($role_names as $rid => $role){
			$data = isset($form['account_fields'][$rid]) && isset($form['account_fields'][$rid][$name]) ? drupal_render($form['account_fields'][$rid][$name]) : '-';
			$row[] = array(
				'data' => $data,
				'class' => 'checkbox'
			);
		}

		$rows[] = $row;
	}

	$output = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'account_fields')));
	$output .= drupal_render_children($form);

	return $output;
}