using System;
using System.Drawing;
using System.Windows.Forms;

namespace Aga.Controls.Tree.NodeControls
{
    public class NodeCategoryTitle : NodeTextBox
    {
        public VerticalAlignment FirstRowVerticalAlign { get; set; }

        public override void Draw(TreeNodeAdv node, DrawContext context)
        {
            if (context.CurrentEditorOwner == this && node == Parent.CurrentNode)
                return;

            var label = GetLabel(node);
            var textBounds = GetBounds(node, context);
            var focusRect = new Rectangle(textBounds.X, context.Bounds.Y,
                textBounds.Width - 1, context.Bounds.Height - 1);

            Brush backgroundBrush;
            Color textColor;
            Font font;
            CreateBrushes(node, context, label, out backgroundBrush, out textColor, out font, ref label);

            //if (backgroundBrush != null)
            //    context.Graphics.FillRectangle(backgroundBrush, focusRect);

            var focusPen = new Pen(SystemColors.Highlight);
            focusPen.Color = context.DrawSelection == DrawSelectionMode.None ? 
                SystemColors.ControlText : SystemColors.InactiveCaption;

            //focusPen.Color = SystemColors.Highlight;
            //context.Graphics.DrawRectangle(focusPen, focusRect);
            
            if (UseCompatibleTextRendering)
                TextRenderer.DrawText(context.Graphics, label, font, textBounds, textColor, _formatFlags);
            else
                context.Graphics.DrawString(label, font, GetBrush(textColor), textBounds, _format);
        }

        private bool IsCategoryNode(TreeNodeAdv node)
        {
            return node.Tag is Node && ((Node)node.Tag).IsCategoryNode;
        }

        protected override Rectangle GetBounds(TreeNodeAdv node, DrawContext context)
        {
            var r = context.Bounds;
            var s = GetActualSize(node, context);
            var bs = new Size(r.Width - LeftMargin, Math.Min(r.Height, s.Height));
            switch (IsFirstRow(node) ? FirstRowVerticalAlign : VerticalAlign)
            {
                case VerticalAlignment.Top:
                    return new Rectangle(new Point(r.X + LeftMargin, r.Y + 4), bs);
                case VerticalAlignment.Bottom:
                    return new Rectangle(new Point(r.X + LeftMargin, r.Bottom - s.Height), bs);
                default:
                    return new Rectangle(new Point(r.X + LeftMargin, (r.Y + (r.Height - s.Height) / 2) + 4), bs);
            }
        }

        private bool IsFirstRow(TreeNodeAdv node)
        {
            return node.Row.Equals(0);
        }

        public override Size MeasureSize(TreeNodeAdv node, DrawContext context)
        {
            var size = GetLabelSize(node, context);
            return size;
        }      

    }
}
