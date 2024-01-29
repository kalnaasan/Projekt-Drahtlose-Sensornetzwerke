import {Component, Inject} from '@angular/core';
import {MAT_DIALOG_DATA, MatDialogRef} from "@angular/material/dialog";

@Component({
  selector: 'app-status-dialog',
  templateUrl: './status-dialog.component.html',
  styleUrls: ['./status-dialog.component.scss']
})
export class StatusDialogComponent {
  public status: string = '';

  constructor(
    public dialogRef: MatDialogRef<StatusDialogComponent>,
    @Inject(MAT_DIALOG_DATA) public data: { sensorName: string, status: string, value: number }[],
  ) {
    for (let i = 0; i < data.length; i++) {
      if (this.data[i].status === 'danger') {
        this.status = 'danger';
        break;
      }
    }
    if (this.status === '') {
      for (let i = 0; i < data.length; i++) {
        if (this.data[i].status === 'warning') {
          this.status = 'warning';
          break;
        }
      }
    } else if (this.status !== 'danger' && this.status !== 'warning') {
      this.status = 'success';
    }
  }

  onNoClick(): void {
    this.dialogRef.close();
  }
}
