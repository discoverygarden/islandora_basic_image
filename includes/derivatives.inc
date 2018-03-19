<?php

/**
 * @file
 * Functions deriving images for objects.
 */

use Drupal\file\FileInterface;
use Drupal\file\Entity\File;

use Drupal\islandora\MimeDetect;

/**
 * Scales the given image.
 *
 * @param Drupal\file\FileInterface $image_file
 *   The image file to scale.
 * @param int $width
 *   The width to scale the derived image to.
 * @param int $height
 *   The height to scale the derived image to.
 * @param bool $upscale
 *   Whether to upscale or not.
 *
 * @return bool
 *   TRUE if successful, FALSE otherwise.
 */
function islandora_basic_image_scale_image(FileInterface $image_file, $width, $height, $upscale = TRUE) {
  $image = Drupal::service('image.factory')->get($image_file->getFileUri());
  try {
    if (!empty($image)) {
      $scale = $image->scale($width, $height);
      if ($scale) {
        return $image->save();
      }
    }
  }
  catch (exception $e) {
    drupal_set_message(t(
        "Basic image failed to scale image with message: '@message'",
        ["@message" => $e->getMessage()]));
    watchdog(
      'islandora_basic_image',
      'Basic image failed to scale image.<br/> With stack: @trace',
      ['@trace' => $e->getTraceAsString()],
      WATCHDOG_ERROR
    );
  }
  return FALSE;
}

/**
 * Adds a datastream to an object.
 *
 * @param AbstractObject $object
 *   The object to add a datastream to.
 * @param string $dsid
 *   The datastream ID to be added.
 * @param Drupal\file\FileInterface $new_file
 *   The file to be added as the datastream content.
 *
 * @return bool|string
 *   TRUE if the datastream was added/modified successfully, the error message
 *   otherwise.
 */
function islandora_basic_image_add_datastream(AbstractObject $object, $dsid, FileInterface $new_file) {
  try {
    $ingest = !isset($object[$dsid]);
    if ($ingest) {
      $ds = $object->constructDatastream($dsid, 'M');
      $ds->label = $dsid;
    }
    else {
      $ds = $object[$dsid];
    }
    if ($ds->mimeType != $new_file->getMimeType()) {
      $ds->mimeType = $new_file->getMimeType();
    }
    $ds->setContentFromFile($new_file->getFileUri());
    if ($ingest) {
      $object->ingestDatastream($ds);
    }
    $new_file->delete();
    return TRUE;
  }
  catch (exception $e) {
    $variables = [
      '@ret' => $e->getTraceAsString(),
    ];
    watchdog('islandora_basic_image', 'Basic image failed to add data stream.<br/>Error: @ret', $variables, WATCHDOG_ERROR);
    $message = $e->getMessage();
    return $message;
  }
}

/**
 * Creates the TN for basic image.
 *
 * @param AbstractObject $object
 *   An AbstractObject representing a Fedora object.
 * @param bool $force
 *   Whether derivative generation is being forced or not.
 *
 * @return array
 *   An array describing the outcome of the thumbnail creation.
 *
 * @see hook_islandora_derivative()
 */
function islandora_basic_image_create_thumbnail(AbstractObject $object, $force = FALSE) {
  $return = '';
  if (!isset($object['TN']) || (isset($object['TN']) && $force === TRUE)) {
    $original_file = islandora_basic_image_copy_obj_datastream($object);
    if ($original_file === FALSE) {
      return islandora_basic_image_no_obj_datastream($object->id);
    }
    else {
      $tn_file = file_copy($original_file['file'], 'temporary://' . $original_file['filename'] . 'TN.' . $original_file['extension']);
      if (islandora_basic_image_scale_image($tn_file, 200, 200)) {
        $added_successfully = islandora_basic_image_add_datastream($object, 'TN', $tn_file);
        if ($added_successfully !== TRUE) {
          $return = islandora_basic_image_failed_adding($added_successfully);
        }
        else {
          $return = [
            'success' => TRUE,
            'messages' => [
              [
                'message' => t('The TN datastream was added successfully for @pid!'),
                'message_sub' => ['@pid' => $object->id],
                'type' => 'dsm',
              ],
            ],
          ];
        }
      }
      else {
        $return = islandora_basic_image_failed_scaling();
      }
    }
    if (isset($original_file)) {
      $original_file['file']->delete();
    }
    return $return;
  }
}

/**
 * Creates the MEDIUM_SIZE derivative for basic image.
 *
 * @param AbstractObject $object
 *   An AbstractObject representing a Fedora object.
 * @param bool $force
 *   Whether derivative generation is being forced or not.
 *
 * @return array
 *   An array describing the outcome of the thumbnail creation.
 *
 * @see hook_islandora_derivative()
 */
function islandora_basic_image_create_medium_size(AbstractObject $object, $force = FALSE) {
  $return = '';
  if (!isset($object['MEDIUM_SIZE']) || (isset($object['MEDIUM_SIZE']) && $force === TRUE)) {
    $original_file = islandora_basic_image_copy_obj_datastream($object);
    if ($original_file === FALSE) {
      return islandora_basic_image_no_obj_datastream($object->id);
    }
    else {
      $medium_file = file_copy($original_file['file'], 'temporary://' . $original_file['filename'] . 'TN.' . $original_file['extension']);
      $upscale = \Drupal::config('islandora_basic_image.settings')->get('islandora_basic_image_upscale_images');
      if (islandora_basic_image_scale_image($medium_file, 500, 700, $upscale)) {
        $added_successfully = islandora_basic_image_add_datastream($object, 'MEDIUM_SIZE', $medium_file);
        if ($added_successfully !== TRUE) {
          $return = islandora_basic_image_failed_adding($added_successfully);
        }
        else {
          $return = [
            'success' => TRUE,
            'messages' => [
              [
                'message' => t('The MEDIUM_SIZE datastream was added successfully for @pid!'),
                'message_sub' => ['@pid' => $object->id],
                'type' => 'dsm',
              ],
            ],
          ];
        }
      }
      else {
        $return = islandora_basic_image_failed_scaling();
      }
    }
    if (isset($original_file)) {
      $original_file['file']->delete();
    }
    return $return;
  }
}

/**
 * Copies the OBJ to the file system.
 *
 * @param AbstractObject $object
 *   An AbstractObject representing an object within Fedora.
 *
 * @return array|bool
 *   FALSE when the object doesn't have the OBJ, an array otherwise.
 */
function islandora_basic_image_copy_obj_datastream(AbstractObject $object) {
  $mime_detect = new MimeDetect();
  if (!isset($object['OBJ'])) {
    return FALSE;
  }
  $ext = $mime_detect->getExtension($object['OBJ']->mimeType);
  $file_name = str_replace(':', '-', $object->id);

  // Create a file object we can save.
  $file_uri = file_create_filename("{$file_name}OBJ.{$ext}", 'temporary://');
  $image = File::create();
  $image->setFileUri($file_uri);
  $image->setFilename($file_name);
  $image->setOwnerId(\Drupal::currentUser()->id());
  $image->setMimeType($object['OBJ']->mimeType);
  $object['OBJ']->getContent($file_uri);
  $image->save();
  return [
    'file' => $image,
    'extension' => $ext,
    'filename' => $file_name,
  ];
}

/**
 * Helper function that returns a message when no OBJ datastream exists.
 *
 * @param string $pid
 *   The PID of the object that derivatives are being generated on.
 *
 * @return array
 *   An array describing the failure of the operation.
 *
 * @see hook_islandora_derivative()
 */
function islandora_basic_image_no_obj_datastream($pid) {
  return [
    'success' => FALSE,
    'messages' => [
      [
        'message' => t('Could not create image derivatives for %s. No image file was uploaded.'),
        'message_sub' => ['%s' => $pid],
        'type' => 'watchdog',
        'severity' => WATCHDOG_ERROR,
      ],
    ],
  ];
}

/**
 * Helper function that returns a message when the image fails to scale.
 *
 * @return array
 *   An array describing the failure of the operation.
 *
 * @see hook_islandora_derivative()
 */
function islandora_basic_image_failed_scaling() {
  return [
    'success' => FALSE,
    'messages' => [
      [
        'message' => t('Unable to scale the image, the datastream was not added'),
        'type' => 'dsm',
        'severity' => 'warning',
      ],
    ],
  ];
}

/**
 * Helper function describing failure when adding/modifying a datastream.
 *
 * @param string $message
 *   The error message returned when attempting to add/modify.
 *
 * @return array
 *   An array describing the failure of the operation.
 *
 * @see hook_islandora_derivative()
 */
function islandora_basic_image_failed_adding($message) {
  return [
    'success' => FALSE,
    'messages' => [
      [
        'message' => $message,
        'type' => 'watchdog',
        'severity' => WATCHDOG_ERROR,
      ],
    ],
  ];
}
