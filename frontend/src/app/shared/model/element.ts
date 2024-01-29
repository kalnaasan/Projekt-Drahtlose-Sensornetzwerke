import {Shape} from "./shape";

export interface Element {
  id: string;
  type: string;
  start: string;
  end: string;
  shape: Shape | null;
  createdAt: Date;
  updatedAt: Date;
}
