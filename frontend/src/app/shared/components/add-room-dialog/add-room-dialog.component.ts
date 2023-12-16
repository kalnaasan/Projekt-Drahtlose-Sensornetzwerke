import {Component, Inject, OnInit} from '@angular/core';
import {FormBuilder, FormGroup, Validators} from "@angular/forms";
import {MAT_DIALOG_DATA, MatDialogRef} from "@angular/material/dialog";

@Component({
  selector: 'app-add-room-dialog',
  templateUrl: './add-room-dialog.component.html',
  styleUrls: ['./add-room-dialog.component.scss']
})
export class AddRoomDialogComponent implements OnInit {



  // @ts-ignore
  public dialogForm: FormGroup;



  ngOnInit(): void {

  }

  onSubmit(): void {
    if (this.dialogForm.valid) {
    }
  }


}
